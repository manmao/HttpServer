#include <unistd.h>
#include <stdio.h>
#include "cgi_handle.h"
#include "http/http_respond.h"

cgi_handle::cgi_handle(int epollfd,int sockfd,struct sockaddr_in address,Config *conf)
{
    this->m_epollfd=epollfd;
    this->m_sockfd=sockfd;
    this->m_address=address;
    this->req=NULL;
    this->rsp=NULL;
    this->config=conf;
}

cgi_handle::~cgi_handle()
{
    delete this->req;
    delete this->rsp;
}

void cgi_handle::removefd(int epollfd,int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

int cgi_handle::process(ServletRegister *sr)
{
   char http_content_buff[5120];
   this->rsp=new HttpResponse(this->m_sockfd);
   while(1)
   {   //接收客户端发送过来的数据
       int buflen=recv(this->m_sockfd,http_content_buff,5120,0);
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR){ //即当buflen<0且errno=EAGAIN时，表示没有数据了。(读/写都是这样)
               //数据读取完成
               break;
           }else{
                string res;
                CHttpResponseMaker::make_400_error(res);
                send(this->m_sockfd,res.c_str(),res.length()+1,0);
                //错误数据
                cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           }
           return -1;
       }
       else if(buflen==0)  //客户端断开连接
       {
           /**将文件描述符从epoll队列中移除**/
           cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           return 0;
       }
       else if(buflen>0) //客户端发送数据过来了
       {
          if(this->req)
          {
              delete this->req;
              this->req=NULL;
          }
          this->req=new HttpRequest(http_content_buff,5120);
          //分发内容,执行请求的地址的内容和方法
          this->req_dispathch(sr);
       }
       memset(http_content_buff,'0',5120);
   }
}


bool cgi_handle::isFile(const string object,string &content_type){

    unsigned int loc=0;
    for(unsigned int i=0;i< object.length();i++)
    {
        if(object[i]=='.')
        {
           loc=i;
        }
    }

    if(loc >=(object.length()-1)||loc ==0 ){
         return false;
    }
    else
    {
        content_type=this->config->type_map[object.substr(loc+1,object.length()-1)];
        if(content_type == "")
            content_type=this->config->type_map["other"];
        return true;
    }
}


void cgi_handle::req_dispathch(ServletRegister *sr)
{
    string uri=this->req->get_uri(); //获取请求url
    string object=this->req->get_object();
    printf("req uri:%s\n",uri.c_str());
    string content_type;

    if(uri=="/")
    {
        uri="/index.html";
        content_type="text/html";
        goto DEAL;
    }
    if(isFile(object,content_type))
    {
        goto DEAL;

    }else{ //如果是接口
        this->req_servlet(sr,uri);
    }

DEAL: //处理请求静态文件
    const char *file_path=(this->config->rootDir+uri).c_str();
    int ret=access(file_path,F_OK);
    if(ret)
    {
        string res;
        CHttpResponseMaker::make_404_error(res);
        send(this->m_sockfd,res.c_str(),res.length()+1,0);
        return ;
    }
    this->req_static_file(file_path,content_type.c_str());
    return ;
}



//处理静态文件请求
void cgi_handle::req_static_file(const char *path,const char* content_type)
{
    printf("%s\n",path);
    int fd=open(path,O_RDONLY);
    assert(fd);
    unsigned long length = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET);
    printf("%ld\n",length); //文件长度

    //发送头部
    char http_buff[1024];
    int head_size=CHttpResponseMaker::make_headers(length,http_buff,content_type);
    send(this->m_sockfd,http_buff,head_size,0);

    //发送数据
    int tmp=0;
    long count=0;
    unsigned char content_buf[1024*50];
    while((tmp=read(fd,content_buf,(sizeof(content_buf)/sizeof(content_buf[0])))))
    {
        if(tmp<=0)
            break;
        send(this->m_sockfd,content_buf,tmp,0);
        count +=tmp;
    }
    printf("read complete!!! %ld\n",count);
    close(fd);
}

//处理Servlet请求
void cgi_handle::req_servlet(ServletRegister *sr,string uri){

    struct rb_root mp=sr->get_url_map();
    //查找url对应的上下文
    struct data_type type;
    type.url=uri;
    struct node_type *node=rbtree_search(&mp,&type);

    //上下文为空，则请求的路径没有
    if(node == NULL)
    {
      string res;
      CHttpResponseMaker::make_404_error(res);
      send(this->m_sockfd,res.c_str(),res.length()+1,0);
      return ;
   }

   Context *context=node->data_content->context;
   //在这里初始化Servlet数据
   context->st->create(this->req,this->rsp);
}





