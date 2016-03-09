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


void  set_socket(int sfd){
	int optval; 	   //���ε�ѡ��ֵ
	socklen_t optlen;  //���ε�ѡ��ֵ�ĳ���
	int err;		   //���÷��ؽ��

    /***���ö˿ں͵�ַ������**/
	optval=1;
	optlen=sizeof(optval);
	err=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,optlen);

	/*******����Nagle�㷨***********/
	optval=1;
	optlen=sizeof(optval);
	err=setsockopt(sfd,IPPROTO_TCP,TCP_NODELAY,(char *)&optval,optlen);

    ///���÷��ͻ�������С
    int iSockBuf = 1024 * 1024;
    while (setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, (void*)&iSockBuf, sizeof(iSockBuf)) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024) break;
    }

    ///���ý��ջ�������С
    iSockBuf = 1024 * 1024;
    while(setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, (void *)&iSockBuf, sizeof(iSockBuf)) < 0)
    {
        iSockBuf -= 1024;
        if (iSockBuf <= 1024) break;
    }
    //����linger,�ر�close����ӳ٣�������TIME_WAIT״̬
    //struct linger{#1:���� 0��1��,#2:ʱ��ֵ������}
    //strcut linger����socket close���Ƿ����WAIT״̬
    struct linger ling= {0, 0};
    if (setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling))!= 0)
    {
        return ;
    }

    setnonblock(sfd);
 }


#ifdef __cplusplus
}
#endif

#endif
