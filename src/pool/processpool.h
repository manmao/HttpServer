﻿#ifndef __PROCESSPOOL_H_
#define __PROCESSPOOL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "threadpool.h"
#include "Config.h"
#include "cgi_handle.h"

/***描述一个子进程的类,m_pipefd是父进程和子进程通信用的管道***/
class process{
public:
	process():m_pid(-1){}
public:
	pid_t m_pid;
	int m_pipefd[2];
};


/******进程池，将他定义为模版是为了代码复用，其模版参数是处理逻辑的类*******/
template<typename T>
class processpool{
private:
	/*将构造函数定义为私有的，因为我们只能通过create静态函数来创建processpool实例*/
	processpool(int listenfd,int process_number=8);
public:
	/**单体模式,以保证程序最多创建一个processpool实例，这是程序正确的处理信号的必要条件**/
	static processpool<T> *create(int listenfd,int process_number=8)
	{
		if(!m_instance)
		{
			m_instance=new processpool<T>(listenfd,process_number);
		}
		return m_instance;
	}

	~processpool()
	{
		delete[] m_sub_process;
	}

	/**启动进程池**/
	void run(Config *config);
private:
	void setup_sig_pipe();
	void run_parent();
	void run_child(Config *config);
private:
	/**进程池允许最大子进程数**/
	static const int MAX_PROCESS_NUMBER=16;

	/** 每个子进程最多能处理的客户数量**/
	static const int USER_PER_PROCESS=65536;

	/**epoll最多能处理事件的个数*/
	static const int MAX_EVENT_NUMBER=10000;

	/**进程池中的进程数**/
	int m_process_number;

	/**子进程在池中的序号，从0开始**/
	int m_idx;

	/**每一个进程都有一个epoll内核事件表，用m_epollfd标识**/
	int m_epollfd;

	/**监听socket是否停止*/
	int m_listenfd;

	/**子进程通过m_stop来决定**/
	int m_stop;

	/** 保存所有子进程的描述信息 **/
	process *m_sub_process;

	/**进程池静态实例**/
	static processpool<T> *m_instance;
};

template <typename T>
processpool<T> *processpool<T>::m_instance=NULL;


/**用于信号处理的管道，以实现统一的事件源，后面称之为信号管道**/
static int sig_pipefd[2];

static int setnonblocking(int fd)
{
	int old_option=fcntl(fd,F_GETFL);
	int new_option=old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}

static void addfd(int epollfd,int fd)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN|EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);//如果设置为非阻塞，那么数据还没有发送出去，send()将会返回
}


/*********************************
**********************************/
static void sig_handler(int sig)
{
	int save_errno=errno;
	int msg=sig;
	send(sig_pipefd[1],(char *)&msg,1,0); //发送到写端
	errno=save_errno;
}

/***********************************
************************************/
static void addsig(int sig,void (handler)(int ),bool restart=true)
{
	struct sigaction sa;
	memset(&sa,'\0',sizeof(sa));
	sa.sa_handler=handler;
	if(restart)
	{
		sa.sa_flags |= SA_RESTART;
	}
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig,&sa,NULL)!=-1);
}


/**进程池构造函数。参数listenfd是监听socket,
他必须在创建进程池之前被创建。否则子进程无法直接引用它**/
template<typename T>
processpool<T>::processpool(int listenfd,int process_number):m_listenfd(listenfd)
	,m_process_number(process_number)
	,m_idx(-1)
	,m_stop(false)
{
	assert((process_number>0)&&(process_number<=MAX_PROCESS_NUMBER));
	m_sub_process=new process[process_number];
	assert(m_sub_process);

	/*****创建process_number个子进程，并建立它们和父进程之间的管道*****/
	for(int i=0;i<process_number;i++)
	{
		int ret=socketpair(PF_UNIX,SOCK_STREAM,0,m_sub_process[i].m_pipefd);
		assert(ret==0);
		m_sub_process[i].m_pid=fork();

		assert(m_sub_process[i].m_pid >= 0);

		if(m_sub_process[i].m_pid > 0) 	//父进程
		{
			close(m_sub_process[i].m_pipefd[1]);//关闭子进程一端
			continue;
		}
		else							 //子进程
		{
			close(m_sub_process[i].m_pipefd[0]);//关闭子进程一端
			m_idx=i;
			break;
		}
	}
}


/**统一事件源**/
template<typename T>
void processpool<T>::setup_sig_pipe()
{
	/** 创建epoll *事件和监听表和信号管道*/
	m_epollfd=epoll_create(MAX_EVENT_NUMBER+2);
	assert(m_epollfd!=-1);

    int ret=socketpair(PF_UNIX,SOCK_STREAM,0,sig_pipefd);
    assert(ret != -1);

	setnonblocking(sig_pipefd[1]);
	addfd(m_epollfd,sig_pipefd[0]); //将信号源的读端口文件描述符 添加到epoll

	/**设置信号处理函数**/
	addsig(SIGCHLD,sig_handler);
	addsig(SIGTERM,sig_handler);
	addsig(SIGINT,sig_handler);
	addsig(SIGPIPE,SIG_IGN);
}



/**
父进程中m_idx值为-1,子进程中m_idx值大于等于0,
我们据此半段接下来要运行的父进程代码还是子进程代码
**/
template<typename T>
void processpool<T>::run(Config *config)
{
	if(m_idx != -1)
	{
		run_child(config);
		return ;
	}
	run_parent();
}

template<typename T>
void processpool<T>::run_child(Config *config)
{
	setup_sig_pipe();

    //设置线程池
    threadpool<cgi_handle> *tp=new threadpool<cgi_handle>();
    assert(tp);

	/*每个子进程都通过其在进程池中序号值m_idx找到与父进程通信的管道*/
	int pipefd=m_sub_process[m_idx].m_pipefd[1];
	addfd(m_epollfd,pipefd);
	epoll_event events[MAX_EVENT_NUMBER];
	T *user=new T[USER_PER_PROCESS];
	assert(user);
	int number=0;
	int ret=-1;

	while(!m_stop)
	{
		number=epoll_wait(m_epollfd,events,MAX_EVENT_NUMBER,-1);
		if((number<0) && (errno != EINTR))
		{
			printf("epoll failuer\n");
			break;
		}
		for(int i=0;i<number;i++)
		{
			int sockfd=events[i].data.fd;
			if((sockfd==pipefd)&&(events[i].events & EPOLLIN))
			{
				int client=0;
				ret=recv(sockfd,(char *)&client,sizeof(client),0);
				if(( (ret<0) && (errno!=EAGAIN)) || ret ==0 )
				{
					continue;
				}
				else
				{
				    while(1)
                    {
                        struct sockaddr_in client_address;
					    socklen_t client_addrlength=sizeof(client_address);
					    int connfd=accept(m_listenfd,(struct sockaddr *)
						        &client_address,&client_addrlength);

					    if(connfd <= 0) //
					    {
						    printf("errno is:%d\n",errno);
                            break;
					    }

					    addfd(m_epollfd,connfd);
                        printf(" %s,%d   connfd:%d\n",__FILE__,__LINE__,connfd);
					    /**模版类T必须实现init方法，以初始化一个客户端连接。
					    我们直接使用connfd来索引逻辑处理对象，提高程序效率**/
					    user[connfd].init(m_epollfd,connfd,client_address,tp,config);
                    }
                    continue;
				}
                printf("request comming !!!!!\n");
			}
			else if((sockfd == sig_pipefd[0]) && (events[i].events&EPOLLIN))
			{
				char signals[1024];
				ret=recv(sig_pipefd[0],signals,sizeof(signals),0);
				if(ret <= 0)
				{
					continue;
				}
				else
				{
					for(int i=0;i<ret;++i)
					{
						switch(signals[i])
						{
							case SIGCHLD:
							{
								pid_t pid;
								int stat;
								while((pid = waitpid(-1,&stat,WNOHANG))>0)
								{
									continue;
								}
								break;
							}
							case SIGTERM:
							case SIGINT:
							{
								m_stop = true;
								break;
							}
							default:
								break;
						}
					}
				}
			}
			/**如果是其他可读数据，那么必然是客户请求到来。调用逻辑处理对象的process方法处理之**/
			else if(events[i].events & EPOLLIN)
			{
				user[sockfd].process();
			}
			else
			{
				continue;
			}
		}
	}

	delete[] user;
	user = NULL;
	close(pipefd);
	//close(m_listenfd);

	/*我们将这句话注释掉，以提醒读者；应该由m_listenfd
		的创建者来关闭这个文件描述符，即所谓的“对象” (比如一个文件描述符，又或者一段堆内存)
		由哪个函数创建，就应该由哪个函数销毁
	*/
	close(m_epollfd);
}

template<typename T>
void processpool<T>::run_parent()
{
	setup_sig_pipe();
	/** 父进程监听 m_listenfd **/
	addfd(m_epollfd,m_listenfd);
    epoll_event events[MAX_EVENT_NUMBER];
	int sub_process_counter=0;
	int new_conn=1;
	int number=0;
	int ret=-1;
     int count=0;
	while(!m_stop)
	{
		number=epoll_wait(m_epollfd,events,MAX_EVENT_NUMBER,-1);

        if((number<0) && (errno != EINTR))
		{
			printf("epoll failuer\n");
			break;
		}

		for(int i=0;i<number;i++)
		{
			int sockfd=events[i].data.fd;
			if(sockfd == m_listenfd)
			{
				/** 如果有新连接到来，就采用Round Robin方式将其分配给一个子进程处理 **/
				int i=sub_process_counter;
				do
				{
					if(m_sub_process[i].m_pid != -1)
						break;
					i=(i+1)%m_process_number;
				}while(i != sub_process_counter);

				if(m_sub_process[i].m_pid == -1)
				{
					m_stop = true;
					break;
				}
				sub_process_counter=(i+1)%m_process_number;
				send(m_sub_process[i].m_pipefd[0],(char *)&new_conn,sizeof(new_conn),0);
				printf("send request to child %d\n",i);
                  count++;
                        printf("连接数量:%d\n",count);
			}
			/*** 处理父进程接收到的信号 ***/
			else if((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN))
			{
				char signals[1024];
				ret=recv(sig_pipefd[0],signals,sizeof(signals),0);
				if(ret <= 0)
				{
					continue;
				}
				else
				{
					for(int i=0;i<ret;i++)
					{
						switch(signals[i])
						{
							case SIGCHLD:
							{
								pid_t pid;
								int stat;
								while((pid=waitpid(-1,&stat,WNOHANG))>0)
								{
									for(int i=0;i<m_process_number;i++)
									{
										/** 如果进程池中第i个子进程退出了，则主进程关闭相应的通信管道。
										并设置相应的m_Pid为-1，以标记该子进程已经退出 **/
										if(m_sub_process[i].m_pid == pid)
										{
											printf("child %d join\n",i);
											close(m_sub_process[i].m_pipefd[0]);
											m_sub_process[i].m_pid=-1;
										}
									}
								}

								/** 如果所有子进程都已经退出，则父进程也退出 **/
								m_stop=true;
								for(int i=0;i<m_process_number;++i)
								{
									if(m_sub_process[i].m_pid != -1)
									{
										m_stop=false;
									}
								}
								break;
							}
							case SIGTERM:
							case SIGINT:
							{
			/*如果父进程接收到终止信号，那么就杀死所有子进程，并等待他们全部结束。
			当然，通知子进程结束更好的方法是向父，
			子进程之间的通信管道发送特殊数据，读者不妨自己实现之*/
								printf("kill all the child now\n");
								for(int i=0;i<m_process_number;i++)
								{
									int pid=m_sub_process[i].m_pid;
									if(pid != -1)
									{
										kill(pid,SIGTERM);
									}
								}
								break;
							}
							default:
								break;
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}

	//close(m_listenfd); 由创建者关闭这个进程
	close(m_epollfd);
}


#endif