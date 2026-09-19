// semshm.c - function definitions
#include "semshm.h"
// create sem set and init with init_val
int creatsem(const char *pathname, int proj_id, int members, int init_val){
    key_t msgkey;
    int sid;
    union semun semopts;

    if((msgkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }

    if((sid = semget(msgkey, members, IPC_CREAT | 0666)) == -1){
        perror("semget error");
        return -1;
    }

    semopts.val = init_val;
    for(int i = 0; i < members; i++){
        if(semctl(sid, i, SETVAL, semopts) == -1){
            perror("semctl SETVAL error");
            return -1;
        }
    }
    return sid;
}
// open existing sem set
int opensem(const char *pathname, int proj_id){
    key_t msgkey;
    int sid;

    if((msgkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }

    if((sid = semget(msgkey, 0, 0)) == -1){
        perror("open semget error");
        return -1;
    }
    return sid;
}
// P op: apply resource, sem - 1, block
int sem_p(int semid, int index){
    struct sembuf sbuf;
    sbuf.sem_num = index;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;        // 0 for block

    if(semop(semid, &sbuf, 1) == -1){
        perror("sem_p error");
        return -1;
    }
    return 0;
}
// V op: release resource, sem + 1, wake up
int sem_v(int semid, int index){
    struct sembuf sbuf;
    sbuf.sem_num = index;
    sbuf.sem_op = 1;
    sbuf.sem_flg = 0;

    if(semop(semid, &sbuf, 1) == -1){
        perror("sem_v error");
        return -1;
    }
    return 0;
}
// delete sem set
int sem_delete(int semid){
    return semctl(semid, 0, IPC_RMID);
}
// create or get shm
int creatshm(const char *pathname, int proj_id, size_t size){
    key_t shmkey;
    int sid;

    if((shmkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }

    if((sid = shmget(shmkey, size, IPC_CREAT | 0666)) == -1){
        perror("shmget error");
        return -1;
    }
    return sid;
}
// delete shm
int deleteshm(int sid){
    return shmctl(sid, IPC_RMID, NULL);
}
