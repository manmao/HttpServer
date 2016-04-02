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
    //У��ͻ��˵�֤��
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
    //ÿ�������ݵ����ʹ���һ�������߳�����ȡ����ִ��
#ifdef _USE_HTTP_SSL
    this->ch=new cgi_handle(this->m_epollfd,this->m_sockfd,this->m_address,this->config,this->ssl);
#else
    this->ch=new cgi_handle(this->m_epollfd,this->m_sockfd,this->m_address,this->config);
#endif
    //��ӵ��̳߳ض��У���threadpool.h�е�void threadpool<T> ::run()��������
    bool ret=this->tp->append(this->ch);
    assert(ret == true);
}



