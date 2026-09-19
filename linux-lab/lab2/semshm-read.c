/* semshm-read.c - client read
    func: open shm and sem - cycle read
    sync: P before read, V before write
    exit: read '#' or P fail to exit
*/
#include "semshm.h"

int main(void){
    int semid, shmid;
    char *shmaddr;

    // 1. open existing shm
    if((shmid = shmget(ftok(".", 57), SHM_SIZE, 0666)) == -1){
        perror("shmget open error");
        return -1;
    }

    // 2. attach shmaddr
    if((shmaddr = (char *)shmat(shmid, NULL, 0)) == (char *)-1){
        perror("shmat error");
        exit(1);
    }

    // 3. open existing sem
    if((semid = opensem("./", 39)) == -1){
        if(shmdt(shmaddr) == -1)
            perror("shmdt error");
        return -1;
    }

    printf("Client (Read) Started\n");

    while(1){
        if(sem_p(semid, 1) == -1)   // P op
            break;

        // check exit flag
        if(shmaddr[0] == '#'){
            printf("read: Received `#`\n");
            sem_v(semid, 0);         // V op
            break;
        }

        printf("read: %s", shmaddr);
        sem_v(semid, 0);             // V op
        usleep(1000);
    }

    if(shmdt(shmaddr) == -1)
        perror("shmdt error");

    printf("Client Exited\n");
    return 0;
}
