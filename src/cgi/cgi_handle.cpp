#include <unistd.h>
#include <stdio.h>
#include "cgi_handle.h"
#include "http/http_respond.h"
#include <sys/sendfile.h>


static
void setblock(int fd){
    int flags;
    flags = fcntl(fd,F_GETFL,0);
    flags &= ~O_NONBLOCK;
    fcntl(fd,F_SETFL,flags);
}

static
int setnonblocking(int fd)
{
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

#ifdef _USE_HTTP_SSL
cgi_handle::cgi_handle(int epollfd,int sockfd,struct sockaddr_in address,Config *conf,SSL *ssl)
{
    this->m_epollfd=epollfd;
    this->m_sockfd=sockfd;
    this->m_address=address;
    this->req=NULL;
    this->rsp=NULL;
    this->config=conf;
    this->ssl=ssl;
}
#endif

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
    if(this->req)
        delete this->req;
   if(this->rsp)
        delete this->rsp;
}

void cgi_handle::removefd(int epollfd,int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
#ifdef _USE_HTTP_SSL
    SSL_shutdown(ssl);
    SSL_free(ssl);
#endif
    close(fd);
}



int cgi_handle::process(ServletRegister *sr)
{
   int buflen=-1;
   char http_content_buff[5120];
   while(1)
   {

//���տͻ��˷��͹���������
    #ifdef _USE_HTTP_SSL
        assert(this->ssl);
        buflen = SSL_read(this->ssl,http_content_buff,5120);
        CHK_SSL(buflen);
    #else
        buflen=recv(this->m_sockfd,http_content_buff,5120,0);
    #endif
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR || errno == EWOULDBLOCK){ //����buflen<0��errno=EAGAINʱ����ʾû�������ˡ�(��/д��������)
               //���ݶ�ȡ���
               break;
           }else{
                //��������
                string res;
                CHttpResponseMaker::make_400_error(res);
            #ifdef _USE_HTTP_SSL
                int err=SSL_write(this->ssl, res.c_str(),res.length()+1);
                CHK_SSL(err);
            #else
                send(this->m_sockfd,res.c_str(),res.length()+1,0);
            #endif
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
   return 0;
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
        return ;
    }

DEAL: //��������̬�ļ�
    const char *file_path=(this->config->rootDir+uri).c_str();
    int ret=access(file_path,F_OK);
    if(ret)
    {
        string res;
        CHttpResponseMaker::make_404_error(res);
     #ifdef _USE_HTTP_SSL
         int err=SSL_write(this->ssl, res.c_str(),res.length()+1);
         CHK_SSL(err);
     #else
         send(this->m_sockfd,res.c_str(),res.length()+1,0);
     #endif

     return ;
    }

    #ifdef _USE_HTTP_SSL
        this->https_req_static_file(file_path,content_type.c_str());
    #else
        this->req_static_file(file_path,content_type.c_str());
    #endif

    return ;
}


//����̬�ļ�����
void cgi_handle::req_static_file(const char *path,const char* content_type)
{
    printf("%s\n",path);
    int fd=open(path,O_RDONLY);
    assert(fd);
    unsigned long long length = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET);
    printf("%lld\n",length); //�ļ�����

    //����ͷ��
    char http_buff[1024]={0};
    int head_size=CHttpResponseMaker::make_headers(length,http_buff,content_type);
    send(this->m_sockfd,http_buff,head_size,0);

     //sendfile��������
     long chuck=1024*200;
     long i=0;
     long total=length;
     long offset=0;
     if(total>chuck)
     {
       setblock(this->m_sockfd);//������ļ����������ļ���������������
     }
     while(1){
          if(total<chuck)
          {
             sendfile(this->m_sockfd,fd,&(offset),total);
             setnonblocking(this->m_sockfd);
             break;
          }
          else{
             sendfile(this->m_sockfd,fd,&(offset),chuck);
          }

          i++;
          total -= chuck;
          offset=i*chuck;
     }

     close(fd);
     printf("read complete!!!\n");
}


#ifdef _USE_HTTP_SSL
//����̬�ļ�����
void cgi_handle::https_req_static_file(const char *path,const char* content_type)
{
    printf("%s\n",path);
    int fd=open(path,O_RDONLY);
    assert(fd);
    unsigned long long length = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET);
    printf("%lld\n",length); //�ļ�����

    //����ͷ��
    char http_buff[1024]={0};
    int head_size=CHttpResponseMaker::make_headers(length,http_buff,content_type);
    SSL_write(this->ssl,http_buff,head_size);

     //sendfile��������
     long chuck=1024*200;
     long i=0;
     long total=length;
     long offset=0;

     unsigned char file_buf[chuck];
     int size=0;

     while(1){
        size=read(fd,file_buf,chuck);
        SSL_write(this->ssl,file_buf,size);

        if(total<chuck)
           break;
        i++;
        total -= chuck;
        offset=i*chuck;
        memset(file_buf,'0',chuck);
     }
     close(fd);
     printf("read complete!!!\n");
}
#endif


//����Servlet����
void cgi_handle::req_servlet(ServletRegister *sr,string uri){

    #ifdef _USE_HTTP_SSL
        this->rsp=new HttpResponse(this->m_sockfd,this->ssl);
    #else
        this->rsp=new HttpResponse(this->m_sockfd);
    #endif

    struct rb_root mp=sr->get_url_map();
    //����url��Ӧ��������
    struct node_data_type type;
    type.url=uri;
    struct node_type *node=rbtree_search(&mp,&type);

    //������Ϊ�գ��������·��û��
    if(node == NULL)
    {
       string res;
       CHttpResponseMaker::make_404_error(res);
 #ifdef _USE_HTTP_SSL
       int err=SSL_write(this->ssl, res.c_str(),res.length()+1);
       CHK_SSL(err);
 #else
       send(this->m_sockfd,res.c_str(),res.length()+1,0);
 #endif
      return ;
   }

   Context *context=node->data_content->context;
   //�������ʼ��Servlet����
   context->get_servlet()->create(this->req,this->rsp);
}

