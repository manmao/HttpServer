#ifndef __SVSHM_XFR_H
#define __SVSHM_XFR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "binary_sems.h"


#define SHM_KEY 0x101
#define SEM_KEY 0x111

#define OBJ_PERMS (S_IRUSR | S_IRGRP | S_IWGRP)


#endif