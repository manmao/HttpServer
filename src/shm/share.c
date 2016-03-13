#include "share.h"

void write_mem(struct data_type *data)
{
	int semid,shmid;
	struct shmseg *shmp;
	union semnu dummy;
	//�����ź�������
	semid = semget(SEM_KEY,2,IPC_CREAT | OBJ_PERMS);

	if(semid == -1)
	{
		errExit("%s:%s:%d\n","semget",__FILE__,__LINE__);
		exit(-1);
	}

	//��ʼ���ź���
	if(initSemAvailable(semid,WRITE_SEM) == -1)
		exit(-1);
	if(initSemInUse(semid,READ_SEM) == -1)
		exit(-1);

	shmid = shmget(SHM_KEY,sizeof(struct shmseg),IPC_CREAT|OBJ_PERMS);
	if(shmid == -1)
		exit(-1);

	shmp = shmat(shmid,NULL,0);
	if(shmp == (void *)-1)
		exit(-1);

	//
	if(reserveSem(semid,WRITE_SEM) == -1)//-1
		exit(-1);

        //д������
        shmp->data=data;

	//
	if(releaseSem(semid,READ_SEM) == -1)//+1
	{
		puts("releaseSem");
		exit(-1);
	}

	if(reserveSem(semid,WRITE_SEM) == -1) //-1
	{
		puts("reserveSem");
		exit(-1);
	}
	if(semctl(semid,0,IPC_RMID,dummy) == -1)
	{
		puts("semctl");
	}

	if(shmdt(shmp) == -1)
	{
		puts("shmp");
		exit(-1);
	}

	if(shmctl(shmid,IPC_RMID,0) == -1)
	{
		puts("shmctl");
		exit(-1);
	}
}

struct data_type* read_mem()
{

    int semid,shmid;
	struct shmseg *shmp;
    struct data_type* data=(struct data_type*)malloc(sizeof(struct data_type));

    ///��ȡ�ź���
	semid = semget(SEM_KEY,0,0);
	if(semid == -1)
	{
		puts("semget");
		exit(-1);
	}

	///��ȡ�����ڴ���
	shmid = shmget(SHM_KEY,0,0);
	if(shmid == -1)
	{
		errExit("%s:%d",__FILE__,__LINE__);
		exit(-1);
	}

	///��ȡ���͵�ַ
	shmp = shmat(shmid,NULL,SHM_RDONLY);

	if(shmp == (void *)-1)
	{
		puts("shmat\n");
		exit(-1);
	}

	///�����ڴ�����
	if(reserveSem(semid,READ_SEM) == -1) //READ_SEM-1
	{
		puts("reserveSem");
		exit(-1);
	}

    //��ȡ����
    memcpy(data,shmp->data,sizeof(struct data_type));


    ///�ͷ��ڴ�����
	if(releaseSem(semid,WRITE_SEM) == -1)//WRITE_SEM+1
	{
			puts("releaseSem\n");
			exit(-1);
	}

	///detach
	if(shmdt(shmp) == -1)
	{
		puts("shmdt\n");
		exit(-1);
	}

	if(releaseSem(semid,WRITE_SEM) == -1) //-1
	{
		puts("reserveSem\n");
		exit(-1);
	}

    return data;
}

