// child.c
/*
    func: signal
    flow: loop output - exit when receive SIGTERM
*/
#include "common.h"

volatile sig_atomic_t running = 1;    //declare running flag for exit main with volatile (ensure atom)

void sigterm_handler(int signo){
    printf("[Child %d] Received SIGTERM (signal %d), exiting...\n", getpid(), signo);
    
    running = 0;
    
}

int main(void){
    // ignore SIGINT to avoid exit when Ctrl + C
    signal(SIGINT, SIG_IGN);

    // register signterm handler
    if(signal(SIGTERM, sigterm_handler) == SIG_ERR){
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("[Child %d] Started, PID = %d\n", getpid(), getpid());
    printf("[Child %d] Working... (send SIGTERM to stop)\n", getpid());

    int i = 0;
    while(running){
        printf("[Child %d] Working... %d\n", getpid(), i++);
        sleep(1);    //output per second
    }
    printf("[Child %d] Exiting gracefully.\n", getpid());
    return 0;
}
