#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string>
#include <fcntl.h>
//#include <>

using std::string;

#include "rdconfig.h"
#include "error_hdr.h"


class Config
{
public:
    Config(string conf="conf/http.conf")
    {
       this->config_path=conf;
       this->init_config();

       int ret=access(const_cast<char *>(conf.c_str()),F_OK); //mode
       if(ret)
       {
            errExit("config file is not exit! %s,%s,%d\n",__FILE__,__func__,__LINE__);
       }
    }
    void init_config()
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
