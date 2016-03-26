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
        rsp->send_redirect("/index.html");
    }
};

#endif

