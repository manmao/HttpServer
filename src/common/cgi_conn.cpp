#include "cgi_conn.h"

cgi_conn::cgi_conn()
{

}

cgi_conn::~cgi_conn()
{

}

void cgi_conn::init(int epollfd,int sockfd,const sockaddr_in& client_addr)
{
    m_epollfd = epollfd;
    m_sockfd =  sockfd;
    m_address = client_addr;
}

void cgi_conn::process()
{

}




