/* semshm-read.c - client read
    func: open shm and sem - cycle read
    sync: P before read, V before write
    exit: read '#' or P fail to exit
*/
#include "semshm.h"

int main(){
    int semid, shmid;
    char* shmaddr;

    // 1. 
    if((shmid = creatshm(".", 57, SHM_SIZE)) == -1)
        return -1;

    // 2. 
    if((shmaddr = (char*)shmat(shmid, NULL, 0)) == (char*)-1){
        perror("shmat error");
        exit(1);
    }

    // 3.
    if((semid = opensem("./", 39)) == -1)
        return -1;

    printf("Client (Read) Started\n");

    while(1){
        sem_p(semid, 1);    // P op

        // check exit flag
        if(shmaddr[0] == '#'){
            printf("read: Received `#`\n");
            sem_v(semid, 0);
            break;
        }

        printf("read: %s", shmaddr);

        sem_v(semid, 0);    // V op
        usleep(1000);
    }

    printf("Client Exited\n");
    return 0;
}
