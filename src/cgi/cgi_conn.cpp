#include "cgi_conn.h"

int cgi_conn::m_epollfd = -1;

cgi_conn::cgi_conn()
{
}

cgi_conn::~cgi_conn()
{
    delete this->ch;
}

void cgi_conn::init(int epollfd,
    int sockfd,
    const sockaddr_in& client_addr,
    threadpool<cgi_handle> *tp)
{
    cgi_conn::m_epollfd = epollfd;
    this->m_sockfd =  sockfd;
    this->m_address = client_addr;
    this->tp=tp;
    this->ch=new cgi_handle(epollfd,sockfd,client_addr);
}

void cgi_conn::process()
{
    bool ret=this->tp->append(this->ch);
    assert(ret == true);
}




