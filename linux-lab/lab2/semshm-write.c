/* semshm-write.c - server write
    func: create shm and sem, read from keyboard, write into memory
    sync: P before write, V after write
    exit: input '#' to delete IPC and exit
*/
#include "semshm.h"

int   g_shmid = -1;
int   g_semid = -1;
char *g_shmaddr = NULL;

// cleanup handler: delete IPC resources on Ctrl+C
void cleanup_handler(int signo){
    printf("\n[Server] Signal %d, cleaning up...\n", signo);
    if(g_shmaddr != NULL) shmdt(g_shmaddr);
    if(g_semid   != -1)   sem_delete(g_semid);
    if(g_shmid   != -1)   deleteshm(g_shmid);
    exit(0);
}

int main(void){
    char write_str[SHM_SIZE];

    // register cleanup for SIGINT / SIGTERM
    signal(SIGINT,  cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    // 1. create shm - ftok + shmget
    if((g_shmid = creatshm(IPC_KEY_PATH, 57, SHM_SIZE)) == -1)
        return -1;

    // 2. map shmaddr - shmat(shmid, NULL, 0)
    if((g_shmaddr = (char *)shmat(g_shmid, NULL, 0)) == (char *)-1){
        perror("shmat error");
        exit(1);
    }
    memset(g_shmaddr, 0, SHM_SIZE);     // clear stale data

    // 3. create sem set with 2 members
    if((g_semid = creatsem(IPC_KEY_PATH, 39, 2, 0)) == -1){
        shmdt(g_shmaddr);
        return -1;
    }

    // sem idx 0: write permit, init 1
    // sem idx 1: read  permit, init 0
    union semun init_val;
    init_val.val = 1;
    if(semctl(g_semid, 0, SETVAL, init_val) == -1)
        perror("semctl set 0 error");
    init_val.val = 0;
    if(semctl(g_semid, 1, SETVAL, init_val) == -1)
        perror("semctl set 1 error");

    printf("Server (write) Started\n");
    printf("Enter text to send ('#' to quit):\n");

    while(1){
        if(sem_p(g_semid, 0) == -1)     // P op
            break;

        printf("write: ");
        if(fgets(write_str, SHM_SIZE, stdin) == NULL)
            break;

        if(write_str[0] == '#'){        // input '#' ready to exit
            strcpy(g_shmaddr, "#");     // signal exit to reader
            sem_v(g_semid, 1);          // V op
            break;
        }

        // write to shm
        strncpy(g_shmaddr, write_str, SHM_SIZE - 1);
        g_shmaddr[SHM_SIZE - 1] = '\0';
        sem_v(g_semid, 1);              // V op
        usleep(1000);
    }

    // clear resource
    sem_delete(g_semid);
    deleteshm(g_shmid);
    if(shmdt(g_shmaddr) == -1)
        perror("shmdt error");

    printf("Server Exited\n");
    return 0;
}
