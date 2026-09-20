// tcp-common.h - common definitions for TCP server/client
#ifndef TCP_COMMON_H
#define TCP_COMMON_H

#include <stdio.h>  // printf, fprintf, perror, fopen, fclose, fwrite, fflush, fgets
#include <stdlib.h>  // exit, atoi, malloc, free
#include <string.h>  // memset, memcpy, strcmp, strncmp, strlen
#include <unistd.h>  // read, write, close, sleep, usleep
#include <errno.h>  // errno
#include <sys/types.h>  // pid_t
#include <sys/socket.h>
// socket, bind, listen, accept, connect, send, recv,
// setsockopt, shutdown, SOL_SOCKET, SO_REUSEADDR, AF_INET, SOCK_STREAM
#include <netinet/in.h>  // inet_ntoa, inet_addr
#include <arpa/inet.h>  // gethostbyname, struct hostent
#include <netdb.h>

#define PORT      3339
#define BUF_SIZE  1024
#define LOG_FILE  "messages.log"

#endif
