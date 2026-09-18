// common.h - public header
#ifdef COMMON_H
#define COMMON_H

#include <stdio.h>  // standard I/O
#include <stdlib.h>  // exit, EXIT_FAILURE
#include <unistd.h>  // UNIX: fork, execl, sleep, getpid
#include <signal.h>  // signal, sigaction, sigemptyset, kill
#include <sys/types.h>  // system types: pid_t
#include <sys/wait.h>  // wait, waitpid, WIFEXITED, WEXITSTATUS, WTREMSIG, WIFSIGNALED
#include <errno.h>  // erron, EINTR

#endif
