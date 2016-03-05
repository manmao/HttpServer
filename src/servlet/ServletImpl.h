#ifndef __SERVERIMPL_LET_H
#define __SERVERIMPL_LET_H

#include <stdio.h>
#include <string>
using std::string;

#include "Servlet.h"

class ServletImpl : public Servlet
{
public:

    void init(){
        printf("init method\n");
    }

    void post(HttpRequest*req,HttpResponse *rsp){
        printf("post method %s\n",req->get_param("man").c_str());
        printf("post method %s\n",req->get_uri().c_str());
        rsp->send_rsp("123231");
    }

    void get(HttpRequest*req,HttpResponse *rsp){
        printf("get method %s\n",req->get_param("man").c_str());
        printf("get method %s\n",req->get_uri().c_str());

        rsp->send_rsp("123231");
    }

    void destroy(){
        printf("destroy method\n");
    }
};

#endif

