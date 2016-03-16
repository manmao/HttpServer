#if 0
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

#include "share.h"

int main(int argc,char *argv[])
{
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

    return 0;
}

#endif
