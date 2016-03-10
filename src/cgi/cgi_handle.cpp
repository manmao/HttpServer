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
   {   //���տͻ��˷��͹���������
       int buflen=recv(this->m_sockfd,http_content_buff,5120,0);
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR){ //����buflen<0��errno=EAGAINʱ����ʾû�������ˡ�(��/д��������)
               //���ݶ�ȡ���
               break;
           }else{
                string res;
                CHttpResponseMaker::make_400_error(res);
                send(this->m_sockfd,res.c_str(),res.length()+1,0);
                //��������
                cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           }
           return -1;
       }
       else if(buflen==0)  //�ͻ��˶Ͽ�����
       {
           /**���ļ���������epoll�������Ƴ�**/
           cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           return 0;
       }
       else if(buflen>0) //�ͻ��˷������ݹ�����
       {
          if(this->req)
          {
              delete this->req;
              this->req=NULL;
          }
          this->req=new HttpRequest(http_content_buff,5120);
          //�ַ�����,ִ������ĵ�ַ�����ݺͷ���
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
    string uri=this->req->get_uri(); //��ȡ����url
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

    }else{ //����ǽӿ�
        this->req_servlet(sr,uri);
    }

DEAL: //��������̬�ļ�
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



//����̬�ļ�����
void cgi_handle::req_static_file(const char *path,const char* content_type)
{
    printf("%s\n",path);
    int fd=open(path,O_RDONLY);
    assert(fd);
    unsigned long length = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET);
    printf("%ld\n",length); //�ļ�����

    //����ͷ��
    char http_buff[1024];
    int head_size=CHttpResponseMaker::make_headers(length,http_buff,content_type);
    send(this->m_sockfd,http_buff,head_size,0);

    //��������
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

//����Servlet����
void cgi_handle::req_servlet(ServletRegister *sr,string uri){

    struct rb_root mp=sr->get_url_map();
    //����url��Ӧ��������
    struct data_type type;
    type.url=uri;
    struct node_type *node=rbtree_search(&mp,&type);

    //������Ϊ�գ��������·��û��
    if(node == NULL)
    {
      string res;
      CHttpResponseMaker::make_404_error(res);
      send(this->m_sockfd,res.c_str(),res.length()+1,0);
      return ;
   }

   Context *context=node->data_content->context;
   //�������ʼ��Servlet����
   context->st->create(this->req,this->rsp);
}





