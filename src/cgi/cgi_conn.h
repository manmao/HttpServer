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

#include "threadpool.h"
#include "cgi_handle.h"
#include "config.h"


/* 用于处理客户CGI 请求的类，它可以作为processpool 类的模版参数 */
class cgi_conn
{
public:
    cgi_conn();
    ~cgi_conn();
    /* 初始化客户连接，清空读缓冲区 */
    void init(int epollfd,int sockfd,const sockaddr_in& client_addr,threadpool<cgi_handle> *tp,Config *config);
    void process();
private:
    static int m_epollfd;
    int m_sockfd;
    sockaddr_in m_address;
    threadpool<cgi_handle> *tp;
    cgi_handle *ch;
private:
    Config *config;

};

#endif
