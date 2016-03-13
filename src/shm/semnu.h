#ifndef __SEMNU_H_
#define __SEMNU_H_

#include <sys/types.h>
#include <sys/sem.h>

union semnu{
	int 			val;
	struct semid_ds *buf;
	unsigned short 	*array;
#if defined (__linux__)
	struct seminfo* __buf;
#endif
};

#endif