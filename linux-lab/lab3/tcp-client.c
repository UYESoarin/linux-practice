/* tcp-client.c - TCP client: send lines, receive echo
   run: ./tcp-client <host> <port>
   e.g.: ./tcp-client 127.0.0.1 3339
*/
#include "tcp-common.h"

// send all data(dest socket fd, buffer, Byte num) return actual sent Byte num
static int send_all(int fd, const char *buf, int len) {
    int total = 0;
    while (total < len) {
        // send: len-total Byte from addr buf+total
        int n = send(fd, buf + total, len - total, 0);
        if (n <= 0) {
            perror("send error");
            return -1;
        }
        total += n;
    }
    return total;
}

// recv line(source socket fd, buffer, buffer size) return received Byte num
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
    buf[total] = '\0';                  // string normally end
    return total;
}

int main(int argc, char *argv[]) {
    int                sockfd;          // client socket fd
    struct sockaddr_in server_addr;     // server addr struct
    struct hostent    *host;            // host info from gethostbyname
    int                portnumber;      // port number
    char               buf[BUF_SIZE];   // send buffer
    char               rebuf[BUF_SIZE]; // recv buffer
    int                n;               // recv Byte

    // 0. check args(argv[1]=host, argv[2]=port)
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    // resolve hostname(string) return hostent* or NULL
    // note: gethostbyname is legacy, getaddrinfo is modern replacement
    if ((host = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "gethostbyname error\n");
        exit(1);
    }
    portnumber = atoi(argv[2]);         // string port to int

    // 1. create socket(IPv4, TCP, default) return socket fd
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // 2. fill server addr(sin_family, sin_port=htons, sin_addr=host IP)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(portnumber);
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    // 3. connect(client fd, server addr, addr size) do 3-way handshake
    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect error");
        exit(1);
    }
    printf("Connected to %s:%d\n", argv[1], portnumber);
    printf("Type messages ('quit' to exit):\n");

    // 4. loop: read stdin, send, recv echo
    while (1) {
        printf("> ");
        // fgets(buffer, size, stdin) read one line with '\n'
        if (fgets(buf, BUF_SIZE, stdin) == NULL) break;

        // strict quit check(not match "quit123")
        if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "quit") == 0) break;

        int len = strlen(buf);
        if (send_all(sockfd, buf, len) == -1) break;

        n = recv_line(sockfd, rebuf, BUF_SIZE);
        if (n <= 0) {
            if (n == 0) printf("Server closed connection.\n");
            else        perror("recv error");
            break;
        }
        printf("Server echo: %s", rebuf);
    }

    // 5. cleanup
    close(sockfd);
    printf("Client exited.\n");
    return 0;
}
