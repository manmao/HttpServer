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
<<<<<<< HEAD
        rsp->send("servlet1");
=======
        rsp->send_rsp("servlet1");
>>>>>>> 60ea385fa026eea106ae80abbcd836f0c6830038
    }
    void get(HttpRequest*req,HttpResponse *rsp){
        printf("get method %s\n",req->get_param("man").c_str());
        printf("get method %s\n",req->get_uri().c_str());
<<<<<<< HEAD
        rsp->send(req->get_param("man")+"      servlet1");
=======
        rsp->send_rsp(req->get_param("man")+"      servlet1");
>>>>>>> 60ea385fa026eea106ae80abbcd836f0c6830038
    }
};

#endif

