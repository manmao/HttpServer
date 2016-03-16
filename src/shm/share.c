#include "share.h"

void init_sem(struct share_context* context,enum MEM_OP_TYPE type,const char *file_path)
{

    int sem_key=ftok(file_path,0x111);

    int semid;

    if(type == MEM_WRITE){
        //创建信号量集合
        semid=semget(sem_key,2,IPC_CREAT | IPC_EXCL | 0666);
        if(semid == -1)
	    {
	        if(errno==EEXIST)
                semid=semget(sem_key,0,0666);
            else
		        errExit("write semget %s:%d\n",__FILE__,__LINE__);
	    }
        //初始化信号量
	    if(initSemAvailable(semid,WRITE_SEM) == -1)
		    exit(-1);
	    if(initSemInUse(semid,READ_SEM) == -1)
		    exit(-1);

    }else if(type == MEM_READ){
        ///获取信号量
	    semid = semget(sem_key,0,0);
	    if(semid == -1)
	    {
		     errExit("read semget>>>%s:%d",__FILE__,__LINE__);
		    exit(-1);
	    }
    }

    context->semid=semid;
}

void init_shm(struct share_context* context,enum MEM_OP_TYPE type,const char *file_path)
{
       int shm_key=ftok(file_path,0x101);

       int shmid;
       struct shmseg *shmp;
       if(type == MEM_WRITE){

          shmid = shmget(shm_key,sizeof(struct shmseg),IPC_CREAT|IPC_EXCL|0666);
          if(shmid == -1){
              if(errno == EEXIST)
                 shmid = shmget(shm_key,0,0666);
              else
                 errExit("write shmget %s:%d",__FILE__,__LINE__);
          }


          shmp = shmat(shmid,NULL,0);
	      if(shmp == (void *)-1)
		      errExit("write shmat %s:%d",__FILE__,__LINE__);

       }else if(type == MEM_READ){
           ///获取共享内存句柄
	       shmid = shmget(shm_key,0,0);
	       if(shmid == -1)
               errExit("read shmget %s:%d",__FILE__,__LINE__);


           ///获取传送地址
	       shmp = shmat(shmid,NULL,SHM_RDONLY);
	       if(shmp == (void *)-1)
                errExit("read shmat>>%s:%d",__FILE__,__LINE__);

       }

       context->shmid=shmid;
       context->shmp=shmp;

}


void write_mem(struct share_context* context,struct shm_data_type data)
{
	//
	if(reserveSem(context->semid,WRITE_SEM) == -1)//-1
        errExit("write reserveSem >>%s:%d",__FILE__,__LINE__);

      //写入数据
      // memcpy(shmp->data,data,sizeof(struct node_data_type));
      context->shmp->data=data;

	//
	if(releaseSem(context->semid,READ_SEM) == -1)//+1
	    errExit("write releaseSem >>%s:%d",__FILE__,__LINE__);

}


struct shm_data_type read_mem(struct share_context* context)
{
    struct shm_data_type data;
	///锁定内存区域
	if(reserveSem(context->semid,READ_SEM) == -1) //READ_SEM-1
	    errExit("read reserveSem >>%s:%d",__FILE__,__LINE__);

    //读取数据
    //memcpy(data,shmp->data,sizeof(struct node_data_type));
     data=context->shmp->data;

    ///释放内存区域
	if(releaseSem(context->semid,WRITE_SEM) == -1)//WRITE_SEM+1
	    errExit("read releaseSem >>%s:%d",__FILE__,__LINE__);


    return data;

}



void destroy_sem(struct share_context* context,enum MEM_OP_TYPE type)
{
    if(type == MEM_WRITE){
        union semnu dummy;
        if(semctl(context->semid,0,IPC_RMID,dummy) == -1)
            errExit("write  semctl >>%s:%d",__FILE__,__LINE__);
    }
}



void destroy_shm(struct share_context* context,enum MEM_OP_TYPE type)
{

    ///detach
    if(shmdt(context->shmp) == -1)
           errExit("shmdt>>%s:%d",__FILE__,__LINE__);

    if(type == MEM_WRITE){
        if(shmctl(context->shmid,IPC_RMID,0) == -1)
	        errExit("write shmctl >>%s:%d",__FILE__,__LINE__);

    }
}


