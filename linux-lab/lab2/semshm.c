// semshm.c - func definition
#include "semshm.h"

// create sem set
int creatsem(const char* pathname, int proj_id, int members, int init_val){
    key_t msgkey;
    int sid;
    union semun semopts;

    // ftok: generate unique ipc key based on dirpath and project id
    if((msgkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }
    // semget: create or open sem set return id
    if((sid = semget(msgkey, members, IPC_CREAT | 0666)) == -1){    // ipc key + semnum + create when not exist | power for all users
        perror("semget error");
        return -1;
    }
    // semctl - SETVAL: init each sem val with semopts (declare struct semun)
    semopts.val = init_val;
    for(int i=0;i<members;i++){
        // int semctl(int semid, int semnum, int cmd, ...)
        if(semctl(sid, i, SETVAL, semopts)){
            perror("semctl SETVAL error");
            return -1;
        }
    }
    return sid;
}

// open existing sem set
int opensem(const char* pathname, int proj_id){
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

// P op
int sem_p(int semid, int index){
    struct sembuf sbuf; // op: sembuf {sem_num, sem_op, sem_flg}
    sbuf.sem_num = index;
    sbuf.sem_op = -1;    // -1 = P
    sbuf.sem_flg = 0;   // 0 for block
    if(semop(semid, &sbuf, 1) == -1){ // 1 for op num
        perror("sem_p error");
        return -1;
    }
    return 0;
}

// V op
int sem_v(int semid, int index){
    struct sembuf sbuf;
    sbuf.sem_num = index;
    sbuf.sem_op = 1;    // +1 = V
    sbuf.sem_flg = 0;
    if(semop(semid, &sbuf, 1) == -1){
        perror("sem_v error");
        return -1;
    }
    return 0;
}

// delete sem set
int sem_delete(int semid){
    // igonre semnum (0) and semopts when cmd = IPC_RMID to del total sem set
    return semctl(semid, 0, IPC_RMID); 
}

// create shm
int creatshm(char* pathname, int proj_id, size_t size){
    key_t shmkey;
    int sid;
    if((shmkey = ftok(pathname, proj_id)) == -1){
        perror("ftok error");
        return -1;
    }
    // shmget: create or open shm with size return id
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
