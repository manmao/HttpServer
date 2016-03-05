#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <malloc.h>
#include <string>
#include "Context.h"
#include "ex-rbtree.h"

#include "Servlet.h"
#include "ServletImpl.h"

using std::string;

class Config
{
public:
    Config()
    {
       this->url_map=rbtree_init();
    }

    //×¢²áÒ»¸öServlet
    void register_servlet(string url,Servlet *servlet)
    {
        Context *context=new Context();
        context->st=servlet;
        struct data_type *content=new data_type;
        content->context=context;
        content->url=url;
        rbtree_insert(&this->url_map,content);
    }

    struct rb_root get_url_map()
    {
        return this->url_map;
    }
private:
    struct rb_root url_map;
};



#endif
