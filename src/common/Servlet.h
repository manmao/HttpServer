#ifndef __SERVER_LET_H
#define __SERVER_LET_H
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
#include "HttpRequest.h"
#include "HttpResponse.h"

class Servlet{
public:
    Servlet();
    ~Servlet();
    void create(CHttpParser *parser,int sockfd);//放入构造数据，解析req
public:
    virtual void init()=0;
    virtual void post(HttpRequest*req,HttpResponse *rsp)=0;
    virtual void get(HttpRequest *req,HttpResponse *rsp)=0;
    virtual void destroy()=0;
private:
    int m_sockfd;
};

#endif
