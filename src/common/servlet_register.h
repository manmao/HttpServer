#ifndef __SERVLETREGISTER_H_
#define __SERVLETREGISTER_H_

#include "context.h"
#include "ex-rbtree.h"
#include "Servlet.h"

class ServletRegister{
public:
    ServletRegister()
    {
          this->url_map=rbtree_init();
    }

    struct rb_root get_url_map()
    {
        return this->url_map;
    }

    //×¢²áÒ»¸öServlet
    void register_servlet(string url,Servlet *servlet)
    {
          Context *context=new Context();
          context->set_servlet(servlet);
          struct node_data_type *content=new node_data_type;
          content->context=context;
          content->url=url;
          rbtree_insert(&this->url_map,content);
    }

private:
    struct rb_root url_map;


};

#endif
