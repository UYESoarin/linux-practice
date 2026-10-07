/* client.c - streaming text client
   send user input, receive stream char by char (typewriter effect)
   run: ./client <host> <port>
   e.g.: ./client 127.0.0.1 3339
*/
#include "common.h"

int main(int argc, char *argv[]) {
    int                sockfd;
    struct sockaddr_in server_addr;
    struct hostent    *host;
    int                portnumber;
    char               buf[BUF_SIZE];
    char               ch;

    // check args(argv[1]=host, argv[2]=port)
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    // resolve hostname
    if ((host = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "gethostbyname error\n");
        exit(1);
    }
    portnumber = atoi(argv[2]);

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // fill server addr and connect
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(portnumber);
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect error");
        exit(1);
    }
    printf("[Client] connected to %s:%d (pid=%d)\n",
           argv[1], portnumber, getpid());

    // receive welcome message (streamed char by char)
    while (recv(sockfd, &ch, 1, 0) > 0) {
        putchar(ch);
        fflush(stdout);
        if (ch == '\n') break;
    }

    // main loop: read stdin, send, stream receive
    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(buf, BUF_SIZE, stdin) == NULL) break;

        // strict quit check(not match "quit123")
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "quit") == 0) {
            send_all(sockfd, "quit\n", 5);
            break;
        }

        // send whole line
        if (send_all(sockfd, buf, strlen(buf)) == -1) break;

        // stream receive until '\n'
        while (1) {
            int n = recv(sockfd, &ch, 1, 0);
            if (n <= 0) {
                printf("\n[Client] server closed connection\n");
                close(sockfd);
                return 0;
            }
            putchar(ch);
            fflush(stdout);
            if (ch == '\n') break;
        }
    }

    close(sockfd);
    printf("[Client] exited\n");
    return 0;
}
