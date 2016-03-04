#ifndef __CGI_HANDLE_H_
#define __CGI_HANDLE_H_

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "http/http_util.h"

class cgi_handle{
public:
    cgi_handle(int epollfd,int sockfd,struct sockaddr_in address);
    ~cgi_handle();
    int process();
    void req_dispathch();
private:
   static void removefd(int epollfd,int fd);
private:
    int m_epollfd;
    int m_sockfd;
    struct sockaddr_in m_address;
private:
    CHttpParser *parser;
};




#endif