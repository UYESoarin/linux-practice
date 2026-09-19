/* semshm-write.c - server write
    func: create shm and sem - read from keyboard - write into memory
    sync: P before write, V before read
    exit: input "#" to delete IPC and exit
*/
#include "semshm.h"

int main(void){
    int semid, shmid;
    char *shmaddr;
    char write_str[SHM_SIZE];

    if((shmid = creatshm(".", 57, SHM_SIZE)) == -1)
        return -1;

    if((shmaddr = (char *)shmat(shmid, NULL, 0)) == (char *)-1){
        perror("shmat error");
        exit(1);
    }

    if((semid = creatsem("./", 39, 2, 0)) == -1){
        if(shmdt(shmaddr) == -1)
            perror("shmdt error");
        return -1;
    }

    union semun init_val;
    init_val.val = 1;
    if(semctl(semid, 0, SETVAL, init_val) == -1)
        perror("semctl set 0 error");

    init_val.val = 0;
    if(semctl(semid, 1, SETVAL, init_val) == -1)
        perror("semctl set 1 error");

    printf("Server (write) Started\n");
    printf("Enter text to send ('#' to quit):\n");

    while(1){
        if(sem_p(semid, 0) == -1)
            break;

        printf("write: ");
        if(fgets(write_str, SHM_SIZE, stdin) == NULL)
            break;

        if(write_str[0] == '#'){
            sem_v(semid, 1);
            break;
        }

        strncpy(shmaddr, write_str, SHM_SIZE - 1);
        shmaddr[SHM_SIZE - 1] = '\0';
        sem_v(semid, 1);
        usleep(1000);
    }

    sem_delete(semid);
    deleteshm(shmid);

    if(shmdt(shmaddr) == -1)
        perror("shmdt error");

    printf("Server Exited\n");
    return 0;
}
