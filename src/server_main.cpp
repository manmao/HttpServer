#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>

#include "inet_sockets.h"
#include "processpool.h"
#include "cgi_conn.h"


int main(int argc,char *argv[]){

    int listenfd=inetListen("8080",SOCK_STREAM, NULL);
    assert(listenfd > 0);

    processpool<cgi_conn> *pool = processpool<cgi_conn>::create(listenfd);

    if(pool)
    {
        pool->run();  //exe same time
        delete pool;
    }

    /*����ǰ���ᵽ�ģ�main�����������ļ������� listenfd,��ô���������Թر�֮*/
    close(listenfd);

    return 0;
}

