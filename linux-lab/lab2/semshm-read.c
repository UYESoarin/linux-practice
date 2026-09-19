/* semshm-read.c - client read
    func: open shm and sem - cycle read
    sync: P before read, V before write
    exit: read '#' or P fail to exit
*/
#include "semshm.h"

int main(void){
    int semid, shmid;
    char *shmaddr;

    if((shmid = creatshm(".", 57, SHM_SIZE)) == -1)
        return -1;

    if((shmaddr = (char *)shmat(shmid, NULL, 0)) == (char *)-1){
        perror("shmat error");
        exit(1);
    }

    if((semid = opensem("./", 39)) == -1){
        if(shmdt(shmaddr) == -1)
            perror("shmdt error");
        return -1;
    }

    printf("Client (Read) Started\n");

    while(1){
        if(sem_p(semid, 1) == -1){
            break;
        }

        if(shmaddr[0] == '#'){
            printf("read: Received `#`\n");
            sem_v(semid, 0);
            break;
        }

        printf("read: %s", shmaddr);
        sem_v(semid, 0);
        usleep(1000);
    }

    if(shmdt(shmaddr) == -1)
        perror("shmdt error");

    printf("Client Exited\n");
    return 0;
}
