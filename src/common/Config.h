#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string>
#include "Context.h"
#include "ex-rbtree.h"

#include "rdconfig.h"
#include "Servlet.h"
using std::string;

class Config
{
public:
    Config(string conf="conf/http.conf")
    {
       this->url_map=rbtree_init();
       this->config_path=conf;
       this->read_config();
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

    void read_config()
    {
        char port[10];
        get_config_string(this->config_path.c_str(),"http","port",port);

        char log_path[128];
        get_config_string(this->config_path.c_str(),"http","logFilePath",log_path);


        char root_dir[128];
        get_config_string(this->config_path.c_str(),"http","rootDir",root_dir);

        this->listenPort=port;
        this->logPath=log_path;
        this->rootDir=root_dir;
    }

private:
    struct rb_root url_map;

public:
    string logPath;
    string listenPort;
    string rootDir;
    string config_path;

};

#endif
