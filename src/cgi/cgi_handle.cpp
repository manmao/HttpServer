#include "cgi_handle.h"
#include "hash.h"
#include "url_hash.h"
#include "ServletImpl.h"

cgi_handle::cgi_handle(int epollfd,int sockfd,struct sockaddr_in address)
{
    this->m_epollfd=epollfd;
    this->m_sockfd=sockfd;
    this->m_address=address;
    this->parser=NULL;
}

cgi_handle::~cgi_handle()
{

}

void cgi_handle::removefd(int epollfd,int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

int cgi_handle::process()
{
   char http_content_buff[5120];
   while(1)
   {       //接收客户端发送过来的数据
       int buflen=recv(this->m_sockfd,http_content_buff,5120,0);
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR){ //即当buflen<0且errno=EAGAIN时，表示没有数据了。(读/写都是这样)

           }else{

           }
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
          this->parser=new CHttpParser(http_content_buff,5120);

          int idx=JSHash("/man/ma");
          url_mp[idx]=new ServletImpl();

          //分发内容,执行请求的地址的内容和方法
          this->req_dispathch();
       }
   }
}

void cgi_handle::req_dispathch()
{
   printf("%s\n",this->parser->get_uri().c_str());
   string uri=this->parser->get_uri();
   //根据uri找到Servlet类，然后根据POST，GET 执行方法

   int idx=JSHash(uri.c_str());

   if(url_mp[idx]!= NULL)
   {
        Servlet *s=url_mp[idx];
        if(this->parser->get_http_method() == HTTP_UTIL_METHOD_GET){
           s->get();
        }
        if(this->parser->get_http_method() == HTTP_UTIL_METHOD_POST){
           s->post();
        }
   }
   else{
        //请求路径不存在
   }

}






