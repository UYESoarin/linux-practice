// common.h - common definitions for lab4
#ifndef COMMON_H
#define COMMON_H

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

#define PORT       3339
#define BUF_SIZE   1024
#define CORPUS_FILE "corpus.txt"
#define LOG_DIR    "logs"

// send all data(fd, buf, len) return actual sent Byte num
static int send_all(int fd, const char *buf, int len) {
    int total = 0;
    while (total < len) {
        int n = send(fd, buf + total, len - total, 0);
        if (n <= 0) {
            perror("send error");
            return -1;
        }
        total += n;
    }
    return total;
}

// recv line(fd, buf, size) return received Byte num
// read per Byte to recognize '\n' boundary
static int recv_line(int fd, char *buf, int size) {
    int total = 0;
    while (total < size - 1) {
        int n = recv(fd, buf + total, 1, 0);
        if (n <= 0) {
            if (total == 0) return n;   // 0=peer close, -1=error
            break;
        }
        if (buf[total] == '\n') {
            total++;
            break;
        }
        total++;
    }
    buf[total] = '\0';
    return total;
}

#endif
