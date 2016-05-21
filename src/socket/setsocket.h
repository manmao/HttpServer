#ifndef __SETSOCKET_H_
#define __SETSOCKET_H_
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <error.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>


#ifdef __cplusplus
    extern "C"{
#endif

static void setnonblock(int fd)
{
  	int flags;
 	flags = fcntl(fd, F_GETFL);
  	flags |= O_NONBLOCK;
  	fcntl(fd, F_SETFL, flags);
}


int set_socket(int sfd){

    int optval; 	   //
	socklen_t optlen;  //
	int err;		   //


	optval=1;
	optlen=sizeof(optval);
	err=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,optlen);

	optval=1;
	optlen=sizeof(optval);
	err=setsockopt(sfd,IPPROTO_TCP,TCP_NODELAY,(char *)&optval,optlen);


    int iSockBuf = 1024 * 1024;
    while ((err=setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, (void*)&iSockBuf, sizeof(iSockBuf))) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024)
            break;
    }

    iSockBuf = 1024 * 1024;
    while((err=setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, (void *)&iSockBuf, sizeof(iSockBuf))) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024) break;
    }

    struct linger ling= {0, 0};
    if ((err=setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling)))!= 0)
    {
        return err;
    }
    setnonblock(sfd);

    return err;
 }


#ifdef __cplusplus
}
#endif

#endif
