#ifndef __BINARY_SEMS_H_
#define __BINARY_SEMS_H_

#ifdef __cplusplus
    extern "C"{
#endif

extern int initSemAvailable(int semId,int semNum);

extern int initSemInUse(int semId,int semNum);

extern int reserveSem(int semId,int semNum);

extern int releaseSem(int semId,int semNum);


#ifdef __cplusplus
}
#endif
#endif