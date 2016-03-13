#ifndef __SHARE_H_
#define __SHARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semnu.h"
#include "svshm_xfr.h"
#include "error_hdr.h"

//¹²ÏíÊý¾Ý
struct data_type{
    int count;
    char data[1024];
};

struct shmseg{
    struct data_type *data;
};


#ifdef __cplusplus
    extern "C"{
#endif

extern void write_mem(struct data_type *data);
extern struct data_type* read_mem();


#ifdef __cplusplus
}
#endif


#endif
