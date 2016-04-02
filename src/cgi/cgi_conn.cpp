#include "cgi_conn.h"

int cgi_conn::m_epollfd = -1;

cgi_conn::cgi_conn()
{
}

cgi_conn::~cgi_conn()
{

}


#ifdef _USE_HTTP_SSL
void cgi_conn::createSSL()
{
    this->ssl=createSSLInstence(this->config->ctx,this->m_sockfd);
    //校验客户端的证书
   //getClientCert(this->ssl);
}
#endif

void cgi_conn::init(int epollfd,
    int sockfd,
    const sockaddr_in& client_addr,
    threadpool<cgi_handle> *tp,
    Config *config)
{
    cgi_conn::m_epollfd = epollfd;
    this->m_sockfd =  sockfd;
    this->m_address = client_addr;
    this->tp=tp;
    this->config=config;
#ifdef _USE_HTTP_SSL
    this->createSSL();
#endif
}


void cgi_conn::process()
{
    //每次有数据到来就创建一个处理线程来读取数据执行
#ifdef _USE_HTTP_SSL
    this->ch=new cgi_handle(this->m_epollfd,this->m_sockfd,this->m_address,this->config,this->ssl);
#else
    this->ch=new cgi_handle(this->m_epollfd,this->m_sockfd,this->m_address,this->config);
#endif
    //添加到线程池队列，在threadpool.h中的void threadpool<T> ::run()弹出队列
    bool ret=this->tp->append(this->ch);
    assert(ret == true);
}



