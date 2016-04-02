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
        rsp->set_header("Server","HTTPServer/1.3.14(Unix) ");
        rsp->set_header("Last-modified","Tue,17Apr200106:46:28GMT");
        rsp->set_header("Last-modified","Tue");
        rsp->set_char_encode("gbk");
        rsp->send_reply("ÂúÃ¯");
    }
};

#endif

