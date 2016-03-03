#ifndef __CGI_CONN_H
#define __CGI_CONN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "processpool.h"

/* ���ڴ���ͻ�CGI ������࣬��������Ϊprocesspool ���ģ����� */
class cgi_conn
{
public:
    cgi_conn();
    ~cgi_conn();
    /* ��ʼ���ͻ����ӣ���ն������� */
    void init(int epollfd,int sockfd,const sockaddr_in& client_addr);
    void process();

   }

private:
    static int m_epollfd;
    int m_sockfd;
    sockaddr_in m_address;
};

#endif
