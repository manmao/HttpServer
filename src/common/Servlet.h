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
    void create(HttpRequest*parser,HttpResponse *rsp);//放入构造数据，解析req
public:
<<<<<<< HEAD
    //post method
=======
>>>>>>> 60ea385fa026eea106ae80abbcd836f0c6830038
    virtual void post(HttpRequest*req,HttpResponse *rsp)=0;
    //get method
    virtual void get(HttpRequest *req,HttpResponse *rsp)=0;
private:
    HttpResponse *rsp;
    HttpRequest *req;
};

#endif
