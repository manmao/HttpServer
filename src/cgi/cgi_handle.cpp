��#include "cgi_handle.h"
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
   {   //���տͻ��˷��͹���������
       int buflen=recv(this->m_sockfd,http_content_buff,5120,0);
       if(buflen < 0)
       {
           if(errno== EAGAIN || errno == EINTR){ //����buflen<0��errno=EAGAINʱ����ʾû�������ˡ�(��/д��������)

           }else{
               string res;
               CHttpResponseMaker::make_400_error(res);
               write(this->m_sockfd,res.c_str(),res.length()+1);
           }

           return -1;
       }
       else if(buflen==0)              //�ͻ��˶Ͽ�����
       {
           /**���ļ���������epoll�������Ƴ�**/
           cgi_handle::removefd(this->m_epollfd,this->m_sockfd);
           return 0;
       }
       else if(buflen>0) //�ͻ��˷������ݹ�����
       {
          this->parser=new HttpRequest(http_content_buff,5120);
          //�ַ�����,ִ������ĵ�ַ�����ݺͷ���
          this->req_dispathch(sr);
       }
   }
}

void cgi_handle::req_dispathch(ServletRegister *sr)
{
    string uri=this->parser->get_uri(); //��ȡ����url
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
         write(this->m_sockfd,res.c_str(),res.length()+1);
         return ;
    }

    Context *context=node->data_content->context;
    //�������ʼ��Servlet����
    context->st->create(this->parser,this->m_sockfd);

}


