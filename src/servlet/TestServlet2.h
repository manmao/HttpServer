#ifndef __TESTSERVLET2_H
#define __TESTSERVLET2_H

#include <stdio.h>
#include <string>
using std::string;

#include "Servlet.h"

class TestServlet2 : public Servlet
{
public:
    void post(HttpRequest*req,HttpResponse *rsp){
        printf("post method %s\n",req->get_param("man").c_str());
        printf("post method %s\n",req->get_uri().c_str());
        rsp->send_reply("123231");
    }
    void get(HttpRequest*req,HttpResponse *rsp){
        printf("get method %s\n",req->get_param("man").c_str());
        printf("get method %s\n",req->get_uri().c_str());
        rsp->add_header("Cache-Control","no-cache");
        rsp->send_reply(req->get_param("man"));
    }
};

#endif

