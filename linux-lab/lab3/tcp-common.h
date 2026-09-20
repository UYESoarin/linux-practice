// tcp-common.h - common definitions for TCP server/client
#ifndef TCP_COMMON_H
#define TCP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT      3339
#define BUF_SIZE  1024
#define LOG_FILE  "messages.log"

#endif
