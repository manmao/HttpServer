#ifndef __CONTEXT_H_
#define __CONTEXT_H_

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
#include "Servlet.h"


class Context{
public:
    Context(){
        this->st=NULL;
    }
    ~Context(){}

    Servlet *get_servlet()
    {
        return this->st;
    }

    void set_servlet( Servlet *servlet){
        this->st=servlet;
    }

private:
    Servlet *st;
};

#endif
