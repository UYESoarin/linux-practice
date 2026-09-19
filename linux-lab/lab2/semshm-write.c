/* semshm-write.c - server write
    func: create shm and sem - read from keyboard - write into memory
    sync: P before write, V before read
    exit: input "#" to delete IPC and exit
*/
#include "semshm.h"
int main(){
    int semid, shmid;
    char* shmaddr;
    char write_str[SHM_SIZE];
    // 1. create shm - ftok + shmget + shmctl
    if((shmid = creatshm(".", 57, SHM_SIZE)) == -1)
        return -1;
    // 2. map shmaddr - shmat(shmid, NULL - kernel assign addr, 0 - RW)
    if((shmaddr = (char*)shmat(shmid, 0, 0)) == (char*)-1){
        perror("shmat error");
        exit(1);
    }
    // 3. create sem - ftok + semget + semctl
    if((semid = creatsem("./", 39, 2, 0)) == -1)
        return -1;
    // sem idx: 0 - init 1 (writable), 1 - init 0 (nothing to read)
    union semun init_val;
    init_val.val = 1; semctl(semid, 0, SETVAL, init_val);
    init_val.val = 0; semctl(semid, 1, SETVAL, init_val);
    printf("Server (write) Started\n");
    printf("Enter text to send ('#' to quit):\n");
    while(1){
        sem_p(semid, 0);    // P op: apply write
        printf("write: ");
        if(fgets(write_str, SHM_SIZE, stdin) == NULL){
            break;
        }
        if(write_str[0] == '#'){    // input '#' ready to exit
            sem_v(semid, 1);    // V op: wake up read
            break;
        }
        //write into shm
        strncpy(shmaddr, write_str, SHM_SIZE - 1);
        shmaddr[SHM_SIZE - 1] = '\0';
        sem_v(semid, 1);
        usleep(1000);
    }
    // clear resource
    sem_delete(semid);
    deleteshm(shmid);
    printf("Server Exited\n");
    return 0;
}
