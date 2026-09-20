/* tcp-client.c - TCP client: send user input, receive echo
   run: ./tcp-client <host> <port>
   e.g.: ./tcp-client 127.0.0.1 3339
*/
#include "tcp-common.h"

int main(int argc, char *argv[]) {
    int                sockfd;
    struct sockaddr_in server_addr;
    struct hostent    *host;
    int                portnumber;
    char               buf[BUF_SIZE];
    char               rebuf[BUF_SIZE];
    int                n;

    // 1. check args
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    // 2. resolve host
    if ((host = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "gethostbyname error\n");
        exit(1);
    }
    portnumber = atoi(argv[2]);

    // 3. create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // 4. connect to server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(portnumber);
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect error");
        exit(1);
    }
    printf("Connected to %s:%d\n", argv[1], portnumber);
    printf("Type messages ('quit' to exit):\n");

    // 5. loop: read stdin, send, recv echo
    while (1) {
        printf("> ");
        if (fgets(buf, BUF_SIZE, stdin) == NULL) break;

        if (strncmp(buf, "quit", 4) == 0) break;

        // send to server
        if (send(sockfd, buf, strlen(buf), 0) == -1) {
            perror("send error");
            break;
        }

        // recv echo
        n = recv(sockfd, rebuf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            if (n == 0) printf("Server closed connection.\n");
            else        perror("recv error");
            break;
        }
        rebuf[n] = '\0';
        printf("Server: %s\n", rebuf);
    }

    // 6. cleanup
    close(sockfd);
    printf("Client exited.\n");
    return 0;
}
