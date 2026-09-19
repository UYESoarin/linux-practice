// semshm.c - function definitions
#include "semshm.h"

// create sem set (remove stale one if exists) and init with init_val
int creatsem(const char *pathname, int proj_id, int members, int init_val){
    key_t msgkey;
    int sid;
    union semun semopts;

    if((msgkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }

    // try exclusive create
    if((sid = semget(msgkey, members, IPC_CREAT | IPC_EXCL | 0666)) == -1){
        if(errno == EEXIST){
            // remove stale set
            if((sid = semget(msgkey, 0, 0)) == -1){
                perror("semget open error");
                return -1;
            }
            if(semctl(sid, 0, IPC_RMID) == -1){
                perror("semctl IPC_RMID error");
                return -1;
            }
            // recreate
            if((sid = semget(msgkey, members, IPC_CREAT | 0666)) == -1){
                perror("semget recreate error");
                return -1;
            }
        } else {
            perror("semget error");
            return -1;
        }
    }

    // init every semaphore with init_val
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
    // nsems=0, flags=0 for opening existing set
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
    sbuf.sem_op  = -1;
    sbuf.sem_flg = 0;       // 0 for block

    if(semop(semid, &sbuf, 1) == -1){
        if(errno != EINTR) perror("sem_p error");
        return -1;
    }
    return 0;
}

// V op: release resource, sem + 1, wake up
int sem_v(int semid, int index){
    struct sembuf sbuf;
    sbuf.sem_num = index;
    sbuf.sem_op  = 1;
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

// create shm (remove stale one if exists)
int creatshm(const char *pathname, int proj_id, size_t size){
    key_t shmkey;
    int sid;

    if((shmkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }

    // try exclusive create
    if((sid = shmget(shmkey, size, IPC_CREAT | IPC_EXCL | 0666)) == -1){
        if(errno == EEXIST){
            // remove stale segment
            if((sid = shmget(shmkey, size, 0)) == -1){
                perror("shmget open error");
                return -1;
            }
            if(shmctl(sid, IPC_RMID, NULL) == -1){
                perror("shmctl IPC_RMID error");
                return -1;
            }
            // recreate
            if((sid = shmget(shmkey, size, IPC_CREAT | 0666)) == -1){
                perror("shmget recreate error");
                return -1;
            }
        } else {
            perror("shmget error");
            return -1;
        }
    }
    return sid;
}

// open existing shm (for reader)
int openshm(const char *pathname, int proj_id, size_t size){
    key_t shmkey;
    int sid;

    if((shmkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }
    if((sid = shmget(shmkey, size, 0)) == -1){
        perror("shmget open error");
        return -1;
    }
    return sid;
}

// delete shm
int deleteshm(int sid){
    return shmctl(sid, IPC_RMID, NULL);
}
