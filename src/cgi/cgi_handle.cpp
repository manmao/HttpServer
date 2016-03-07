·#include "cgi_handle.h"
#include "http/http_respond.h"

cgi_handle::cgi_handle(int epollfd,int sockfd,struct sockaddr_in address,Config *conf)
{
    this->m_epollfd=epollfd;
    this->m_sockfd=sockfd;
    this->m_address=address;
    this->parser=NULL;
    this->config=conf;
}

cgi_handle::~cgi_handle()
{

}

void cgi_handle::removefd(int epollfd,int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

int cgi_handle::process(ServletRegister *sr)
{
   char http_content_buff[5120];
   while(1)
   {   //接收客户端发送过来的数据
       int buflen=recv(this->m_sockfd,http_content_buff,5120,0);
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR){ //即当buflen<0且errno=EAGAIN时，表示没有数据了。(读/写都是这样)

           }else{
               string res;
               CHttpResponseMaker::make_400_error(res);
               write(this->m_sockfd,res.c_str(),res.length()+1);
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
          this->parser=new HttpRequest(http_content_buff,5120);
          //分发内容,执行请求的地址的内容和方法
          this->req_dispathch(sr);
       }
   }
}

void cgi_handle::req_dispathch(ServletRegister *sr)
{
    string uri=this->parser->get_uri(); //获取请求url
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
         write(this->m_sockfd,res.c_str(),res.length()+1);
         return ;
    }

    Context *context=node->data_content->context;
    //在这里初始化Servlet数据
    context->st->create(this->parser,this->m_sockfd);

}


