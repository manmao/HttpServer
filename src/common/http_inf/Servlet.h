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
    virtual ~Servlet();
    //创建一个Servlet实例
    void create(HttpRequest*parser,HttpResponse *rsp);
public:
    //post method
    virtual void post(HttpRequest*req,HttpResponse *rsp)=0;
    //get method
    virtual void get(HttpRequest *req,HttpResponse *rsp)=0;
private:
    HttpResponse *rsp;
    HttpRequest *req;
};

#endif
