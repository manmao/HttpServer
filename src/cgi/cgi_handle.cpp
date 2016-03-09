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

           }else{
               string res;
               CHttpResponseMaker::make_400_error(res);
               send(this->m_sockfd,res.c_str(),res.length()+1,0);
           }
            cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           return -1;
       }
       else if(buflen==0)              //客户端断开连接
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


static
bool isHtml(string &uri){
    int length=uri.length();
    if(length < 7)
           return false;
    string last=uri.substr(length-5,5);
    if(last == ".html")
    {
        return true;
    }
    return false;
}

void cgi_handle::req_dispathch(ServletRegister *sr)
{
    string uri=this->req->get_uri(); //获取请求url
    printf("req uri:%s\n",uri.c_str());

    if(uri=="/")
        uri="/index.html";

    if(isHtml(uri)){
        const char *file_path=(this->config->rootDir+uri).c_str();
        int ret=access(file_path,F_OK);
        if(ret)
        {
             string res;
             CHttpResponseMaker::make_404_error(res);
             send(this->m_sockfd,res.c_str(),res.length()+1,0);
             return ;
        }
        this->req_html(file_path);
    }else{
        this->req_servlet(sr,uri);
    }
}

void cgi_handle::req_html(const char *path)
{
    char content_buf[1024*20];

    printf("%s\n",path);

    int fd=open(path,O_RDONLY);
    assert(fd);

    unsigned long length = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET);
    printf("%ld\n",length); //文件长度

    int tmp=0;
    long count=0;
    while(tmp=read(fd,content_buf,(sizeof(content_buf)/sizeof(content_buf[0]))))
    {
        if(tmp<=0)
            break;
        if(count == 0)
        {
             char http_buff[tmp+1024];
             CHttpResponseMaker::make(content_buf,tmp,length,http_buff,tmp+1024, "text/html");
             send(this->m_sockfd,http_buff,tmp+1024,0);
        }else{
             send(this->m_sockfd,content_buf,tmp,0);
        }
        count +=tmp;
    }
    printf("read complete!!! %ld\n",count);
    close(fd);
}

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





