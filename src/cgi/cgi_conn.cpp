#include "cgi_conn.h"

int cgi_conn::m_epollfd = -1;

cgi_conn::cgi_conn()
{
}

cgi_conn::~cgi_conn()
{
}


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
}


void cgi_conn::process()
{
    //ÿ�������ݵ����ʹ���һ�������߳�����ȡ����ִ��
    this->ch=new cgi_handle(this->m_epollfd,this->m_sockfd,this->m_address,this->config);
    //��ӵ��̳߳ض��У���threadpool.h�е�void threadpool<T> ::run()��������
    bool ret=this->tp->append(this->ch);
    assert(ret == true);
}



