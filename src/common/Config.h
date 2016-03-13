#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <string>
#include <map>
#include <fcntl.h>
//#include <>

using std::string;
using std::map;

#include "rdconfig.h"
#include "error_hdr.h"


class Config
{
public:
    Config(string conf="conf/http.conf")
    {
       this->config_path=conf;
       int ret=access(const_cast<char *>(conf.c_str()),F_OK); //mode
       if(ret)
       {
            errExit("config file is not exit! %s,%s,%d\n",__FILE__,__func__,__LINE__);
       }
       this->init_config();
       this->init_type_map();
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

    void init_type_map()
    {
        type_map["html"]="text/html";
        type_map["jpeg"]="image/jpeg";
        type_map["jpg"]="image/jpeg";
        type_map["png"]="image/png";
        type_map["bmp"]="image/bmp";
        type_map["gif"]="image/gif";
        type_map["pdf"]="application/octet-stream";
        type_map["json"]="application/json";
        type_map["mp3"]="audio/mp3";
        type_map["mp4"]="video/mpeg4";
        type_map["zip"]="application/zip";
        type_map["doc"]="application/octet-stream";
        type_map["css"]="text/css";
        type_map["js"]="application/javascript";
        type_map["other"]="application/octet-stream";
    }

    ~Config()
    {

    }
public:
    string logPath;
    string listenPort;
    string rootDir;
    string config_path;
    map<string,string> type_map;


};

#endif
