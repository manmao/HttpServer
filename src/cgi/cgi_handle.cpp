#include "cgi_handle.h"

cgi_handle::cgi_handle(int epollfd,int sockfd,struct sockaddr_in address)
{
    this->m_epollfd=epollfd;
    this->m_sockfd=sockfd;
    this->m_address=address;
}

cgi_handle::~cgi_handle()
{

}


void cgi_handle::process()
{

}



