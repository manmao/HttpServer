#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string>

using std::string;

#include "rdconfig.h"


class Config
{
public:
    Config(string conf="conf/http.conf")
    {
       this->config_path=conf;
       this->read_config();
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

public:
    string logPath;
    string listenPort;
    string rootDir;
    string config_path;

};

#endif
