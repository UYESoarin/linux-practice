// semshm.h
#ifndef SEMSHM_H
#define SEMSHM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_SIZE 1024

// compatible
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
#else
  // union: init sem with semctl
  union semun{
      int val;    //init val
      struct semid_ds* buf;
      unsigned short* array;
  };
#endif

// declaration
int creatsem(const char *pathname, int proj_id, int members, int init_val);
int opensem(const char *pathname, int proj_id);
int sem_p(int semid, int index);
int sem_v(int semid, int index);
int sem_delete(int semid);
int creatshm(char *pathname, int proj_id, size_t size);
int deleteshm(int sid);

#endif
