#ifndef __SHARE_H_
#define __SHARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semnu.h"
#include "svshm_xfr.h"
#include "error_hdr.h"

enum MEM_OP_TYPE{
    MEM_READ, //读内存
    MEM_WRITE //写内存
};

//共享数据单元
struct shm_data_type{
    int count;
    char data[1024];
};

struct shmseg{
    struct shm_data_type data;
};

//
struct share_context{
    int semid;
    int shmid;
    struct shmseg *shmp;
};


#ifdef __cplusplus
    extern "C"{
#endif

//初始化信号量
extern void init_sem(struct share_context* context,enum MEM_OP_TYPE type,const char *file_path);

//初始化共享内存
extern void init_shm(struct share_context* context,enum MEM_OP_TYPE type,const char *file_path);

extern void destroy_sem(struct share_context* context,enum MEM_OP_TYPE type);
extern void destroy_shm(struct share_context* context,enum MEM_OP_TYPE type);

//写入共享内存数据
extern void write_mem(struct share_context* context,struct shm_data_type data);

//读取共享内存数据
extern struct shm_data_type read_mem(struct share_context* context);

#ifdef __cplusplus
}
#endif


#endif
