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
        const char *config_file=this->config_path.c_str();
        char port[10];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("port"),port);

        char log_path[516];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("logFilePath"),log_path);

        char root_dir[516];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("rootDir"),root_dir);

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
