#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>

#include "inet_sockets.h"
#include "processpool.h"
#include "cgi_conn.h"
#include "register.h"

int main(int argc,char *argv[]){

    //��ʼ������
    Config *config=new Config();
    regist_servlet(config);

    int listenfd=inetListen("8080",SOCK_STREAM, NULL);
    assert(listenfd > 0);
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

