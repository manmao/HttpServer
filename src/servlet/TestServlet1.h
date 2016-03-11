#ifndef __TESTSERVLET1_H
#define __TESTSERVLET1_H
#include <stdio.h>
#include <string>
using std::string;

#include "Servlet.h"

class TestServlet1 : public Servlet
{
public:
    void post(HttpRequest*req,HttpResponse *rsp){
        printf("post method %s\n",req->get_param("man").c_str());
        printf("post method %s\n",req->get_uri().c_str());
        rsp->send("servlet1");
    }
    void get(HttpRequest*req,HttpResponse *rsp){
        printf("get method %s\n",req->get_param("man").c_str());
        printf("get method %s\n",req->get_uri().c_str());
        rsp->send(req->get_param("man")+"      servlet1");
    }
};

#endif

