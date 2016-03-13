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

int main(int argc,char *argv[])
{
    //��ʼ������
    Config *config=NULL;
    if(argc == 2){
       config=new Config(argv[1]);
    }else if(argc == 1){
        config=new Config();
    }
    //��ʼ���˿�����
    int listenfd=inetListen(config->listenPort.c_str(),65533, NULL);
    assert(listenfd > 0);
    set_socket(listenfd);

    processpool<cgi_conn> *pool = processpool<cgi_conn>::create(listenfd);
    if(pool)
    {
        pool->run(config);  //exe same time
        delete pool;
    }
    /*����ǰ���ᵽ�ģ�main�����������ļ������� listenfd,��ô���������Թر�֮*/
    close(listenfd);

    delete config;
    return 0;
}

