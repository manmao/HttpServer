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
#include "register.h"

#include "share.h"

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

    //初始化端口连接
    int listenfd=inetListen(config->listenPort.c_str(),config->backlog, NULL);
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

 #if 0


    pid_t pid;
    pid=fork();
    if(pid<0)
        exit(-1);
    if(pid>0){

        struct share_context context;
        init_sem(&context,MEM_WRITE,"/home/linux/man.key");
        init_shm(&context,MEM_WRITE,"/home/linux/man.key");
        struct shm_data_type data;
        int i=0;
        for(;i<100;i++){
            data.count=i;
            write_mem(&context,data);
        }
        sleep(1); //
        destroy_sem(&context,MEM_WRITE);
        destroy_shm(&context,MEM_WRITE);

    }else
    {
        struct share_context context;
        init_sem(&context,MEM_READ,"/home/linux/man.key");
        init_shm(&context,MEM_READ,"/home/linux/man.key");
        int i=0;
        struct shm_data_type data;
        for(;i<100;i++){
            data=read_mem(&context);
            printf("%d\n",data.count);
        }
        destroy_sem(&context,MEM_READ);
        destroy_shm(&context,MEM_READ);

    }
#endif

    return 0;
}

