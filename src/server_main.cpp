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
    //初始化配置
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
    //初始化端口连接
    int listenfd=inetListen(config->https_port.c_str(),config->backlog, NULL);
    printf("***********INIT HTTPS SERVER **************\n");
#else
     //初始化端口连接
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

    //正如前文提到的，main函数创建了文件描述符 listenfd,那么就由它亲自关闭之
    close(listenfd);

    delete config;


    return 0;
}

