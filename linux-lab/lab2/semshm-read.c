/* semshm-read.c - client read
    func: open shm and sem, cycle read
    sync: P before read, V after read
    exit: read '#' or P fail to exit
*/
#include "semshm.h"

int   g_shmid = -1;
int   g_semid = -1;
char *g_shmaddr = NULL;

// cleanup handler: only detach shm (server removes resources)
void cleanup_handler(int signo){
    printf("\n[Client] Signal %d, detaching...\n", signo);
    if(g_shmaddr != NULL) shmdt(g_shmaddr);
    exit(0);
}

int main(void){
    // register cleanup for SIGINT / SIGTERM
    signal(SIGINT,  cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    // 1. open existing shm
    if((g_shmid = openshm(IPC_KEY_PATH, 57, SHM_SIZE)) == -1){
        printf("Error: shm not found, run writer first\n");
        return -1;
    }

    // 2. attach shmaddr
    if((g_shmaddr = (char *)shmat(g_shmid, NULL, 0)) == (char *)-1){
        perror("shmat error");
        exit(1);
    }

    // 3. open existing sem
    if((g_semid = opensem(IPC_KEY_PATH, 39)) == -1){
        printf("Error: sem not found, run writer first\n");
        shmdt(g_shmaddr);
        return -1;
    }

    printf("Client (Read) Started\n");

    while(1){
        if(sem_p(g_semid, 1) == -1)     // P op
            break;

        // check exit flag
        if(g_shmaddr[0] == '#'){
            printf("read: Received '#'\n");
            sem_v(g_semid, 0);          // V op
            break;
        }

        printf("read: %s", g_shmaddr);
        sem_v(g_semid, 0);              // V op
        usleep(1000);
    }

    if(shmdt(g_shmaddr) == -1)
        perror("shmdt error");

    printf("Client Exited\n");
    return 0;
}
