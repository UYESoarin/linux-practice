/* tcp-server.c - TCP server: receive messages, write to file, echo back
   run: ./tcp-server
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
// read per Byte to recognize '\n' boundary
static int recv_line(int fd, char *buf, int size) {
    int total = 0;
    while (total < size - 1) {
        // recv 1 Byte each time to catch '\n'
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

int main(void) {
    int                s_fd, c_fd;       // listen-socket, connect-socket
    struct sockaddr_in s_addr, c_addr;   // server addr, client addr
    socklen_t          c_len;            // client addr size
    char               buf[BUF_SIZE];    // send-recv buffer
    int                n;                // recv Byte
    int                opt = 1;          // setsockopt value
    FILE              *log_fp;           // log file stream

    // 1. create socket(IPv4 protocol, TCP, default) return socket fd
    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // 2. set port reuse(avoid TIME_WAIT bind error on restart)
    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt error");
        exit(1);
    }

    // 3. bind(server fd, addr struct, addr size)
    //    fill: sin_family=AF_INET, sin_addr=INADDR_ANY, sin_port=htons(PORT)
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;            // IPv4
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // listen all interfaces
    s_addr.sin_port        = htons(PORT);        // host to net port
    if (bind(s_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        perror("bind error");
        exit(1);
    }

    // 4. listen(fd, backlog=5) turn to passive socket
    if (listen(s_fd, 5) == -1) {
        perror("listen error");
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    // 5. open log file(name, append mode) return FILE* stream
    if ((log_fp = fopen(LOG_FILE, "a")) == NULL) {
        perror("fopen log error");
        close(s_fd);
        exit(1);
    }

    // 6. cycle accept clients(sequential service, not concurrent)
    while (1) {
        c_len = sizeof(c_addr);
        // accept(listen fd, client addr, addr size) return new connect fd
        if ((c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &c_len)) == -1) {
            perror("accept error");
            continue;   // skip single error link, continue to listen
        }
        printf("Client connected from %s\n", inet_ntoa(c_addr.sin_addr));

        // 7. handle one client requests until disconnect
        while (1) {
            n = recv_line(c_fd, buf, BUF_SIZE);
            if (n <= 0) {
                if (n == 0) printf("Client disconnected.\n");
                else        perror("recv error");
                break;
            }
            printf("Received: %s", buf);

            // write to log file(buffer, size 1 Byte, count n, stream)
            fwrite(buf, 1, n, log_fp);
            fflush(log_fp);             // flush so file is updated immediately

            // echo back original message
            if (send_all(c_fd, buf, n) == -1) {
                break;
            }
        }
        close(c_fd);                    // close this client, back to accept
    }

    // unreachable in normal loop
    fclose(log_fp);
    close(s_fd);
    printf("Server exited.\n");
    return 0;
}
