/* main.c
    func: fork - exec - sigaction
    flow:
        1. parent process: register SIGINT - fork child process
        2. child process: exec child
        3. parent process: wait for Ctrl + C - send SIGTERM to child in signal handler
        4. child process: receive STGTERM - exit
        5. parent process: recycle zombie child process - exit
*/
#include "common.h"

pid_t child_pid = -1;    // global child process PID for signal handler

void sigint_handler(int signo){
    printf("\n[Parent %d] Received SIGINT (signal %d)\n", getpid(), signo);
    if(child_pid > 0){
        printf("[Parent %d] Sending SIGTERM to child %d\n", getpid(), child_pid);
        kill(child_pid, SIGTERM);    //send SIGTERM to child
    }
}

int main(void){
    // 1. register SIGINT with sigaction
    struct sigaction act;
    act.sa_handler = sigint_handler;    // specify handler func
    sigemptyset(&act.sa_mask);    //clear signal mask set
    act.sa_flags = 0;    //default flag

    if(sigaction(SIGINT, &act, NULL)==-1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    printf("[Parent %d] SIGINT handler registered.\n", getpid());

    // 2. fork child process
    child_pid = fork();
    if(child_pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(child_pid == 0){
        // 3. child process: exec child
        printf("[Child %d] About to exec ./child\n", getpid());
        // execl chidl in current directory, replace rest child process
        execl("./child", "child", NULL);
        // execl fail
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // 4. wait for child terminate
    printf("[Parent %d] Created child PID = %d\n", getpid(), child_pid);
    printf("[Parent %d] Press Ctrl + C to terminate child...\n", getpid());
    int status;
    // waitpid (block) return -1 when int or error, return pid (!=-1) when child terminate
    while(waitpid(child_pid, &status, 0) == -1){
        if(errno == EINTR)continue;    // int: errno == EINTR
        perror("waitpid");    // error
        break;
    }
    //analyse child process exit status
    if(WIFEXITED(status)){
        printf("[Parent %d] Child exited normally, code = %d\n", getpid(), WEXITSTATUS(status));
    }else if(WIFSIGNALED(status)){
        printf("[Parent %d] Child killed by signal %d\n", getpid(), WTERMSIG(status));
    }
    printf("[Parent %d] Exiting.\n", getpid());
    return 0;
}
