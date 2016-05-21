#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>


#include "inet_sockets.h"
#include "setsocket.h"
#include "processpool.h"
#include "cgi_conn.h"


int main(int argc,char *argv[])
{
    //新建config
    Config *config=NULL;
    if(argc == 2){
       config=new Config(argv[1]);
    }else if(argc == 1){
       config=new Config();
    }
    config->init_config();

#ifdef _USE_HTTP_SSL
    //init ssl
    config->init_ssl();
    //开启socket监听
    int listenfd=inetListen(config->https_port.c_str(),config->backlog, NULL);
    printf("***********INIT HTTPS SERVER **************\n");
#else
     
     int listenfd=inetListen(config->http_port.c_str(),config->backlog, NULL);
     printf("***********INIT HTTP SERVER**************\n");
#endif

    assert(listenfd > 0);
    set_socket(listenfd);

    processpool<cgi_conn> *pool = processpool<cgi_conn>::create(listenfd,config->procs);
    if(pool)
    {
        pool->run(config);  //exe same time
        delete pool;
    }

    //关闭监听
    close(listenfd);

    delete config;


    return 0;
}

