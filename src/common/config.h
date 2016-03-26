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

#ifdef _USE_HTTP_SSL
  #include "config_ssl.h"
#endif

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
       this->init_type_map();
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


    void init_config()
    {
        const char *config_file=this->config_path.c_str();
        char h_port[10];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("http_port"),h_port);

        char hs_port[10];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("https_port"),hs_port);

        char log_path[516];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("logFilePath"),log_path);

        char root_dir[516];
        get_config_string(config_file,const_cast<char *>("http"),const_cast<char *>("rootDir"),root_dir);

        this->http_port=h_port;
        this->https_port=hs_port;

        this->logPath=log_path;
        this->rootDir=root_dir;

    #ifdef _USE_HTTP_SSL
        char cert_path[516];
        get_config_string(config_file,const_cast<char *>("ssl"),const_cast<char *>("certPath"),cert_path);
        char  key_path[516];
        get_config_string(config_file,const_cast<char *>("ssl"),const_cast<char *>("keyPath"),key_path);

        this->certpath=cert_path;
        this->keypath=key_path;

    #endif
        get_config_int(config_file,const_cast<char *>("system"), const_cast<char *>("procs"),&this->procs);
        get_config_int(config_file,const_cast<char *>("system"), const_cast<char *>("threads"),&this->threads);
        get_config_int(config_file,const_cast<char *>("system"), const_cast<char *>("max_requests"),&this->max_requests);
        get_config_int(config_file,const_cast<char *>("system"), const_cast<char *>("backlog"),&this->backlog);

    }

    void init_ssl(){
#ifdef _USE_HTTP_SSL
     printf("_USE_HTTP_SSL SSL_INIT\n");
     this->ctx=SSL_preliminaries(this->certpath.c_str(),this->keypath.c_str());
#endif
    }

    ~Config()
    {
    #ifdef _USE_HTTP_SSL
         SSL_CTX_free(this->ctx);
    #endif
    }
public:
    string logPath;
    string http_port;
    string https_port;
    string rootDir;
    string config_path;
    map<string,string> type_map;

    int procs;
    int threads;
    int max_requests;
    int backlog;

#ifdef _USE_HTTP_SSL
    SSL_CTX* ctx;
    string certpath;
    string keypath;
#endif


};

#endif
