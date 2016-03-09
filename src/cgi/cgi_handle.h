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

#include "HttpRequest.h"
#include "HttpResponse.h"

#include "Config.h"
#include "ServletRegister.h"

class cgi_handle{
public:
    cgi_handle(int epollfd,int sockfd,struct sockaddr_in address,Config *conf);
    ~cgi_handle();
    int process(ServletRegister *sr);
private:
    void req_dispathch(ServletRegister *sr);
    void req_html(const char *path);
    void req_servlet(ServletRegister *sr,string uri);
private:
   static void removefd(int epollfd,int fd);
private:
    int m_epollfd;
    int m_sockfd;
    struct sockaddr_in m_address;
private:
    HttpRequest* req;
    HttpResponse* rsp;
    Config *config;
};

#endif