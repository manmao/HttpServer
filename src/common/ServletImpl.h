#ifndef __SERVERIMPL_LET_H
#define __SERVERIMPL_LET_H

#include <stdio.h>

class ServletImpl:public Servlet{
public:
    void post(){
        printf("post method\n");
    }
    void get(){
        printf("get method\n");
    }
};

#endif

