#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <malloc.h>
#include <string>
#include "Context.h"
#include "ex-rbtree.h"

#include "ServletImpl.h"

using std::string;

class Config
{
public:
    void register_url_map()
    {
        this->url_map=rbtree_init();
        Context context;
        context.st=new ServletImpl();
        string url="/man/m";
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
