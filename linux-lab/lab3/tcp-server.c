/* tcp-server.c - TCP server: receive messages, write to file, echo back
   run: ./tcp-server
*/
#include "tcp-common.h"

int main(void) {
    int                s_fd, c_fd;
    struct sockaddr_in s_addr, c_addr;
    socklen_t          c_len;
    char               buf[BUF_SIZE];
    int                n;
    FILE              *log_fp;

    // 1. create socket
    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // 2. bind
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // listen all interfaces
    s_addr.sin_port        = htons(PORT);
    if (bind(s_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        perror("bind error");
        exit(1);
    }

    // 3. listen
    if (listen(s_fd, 5) == -1) {
        perror("listen error");
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    // 4. accept
    c_len = sizeof(c_addr);
    if ((c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &c_len)) == -1) {
        perror("accept error");
        exit(1);
    }
    printf("Client connected from %s\n", inet_ntoa(c_addr.sin_addr));

    // 5. open log file for appending
    if ((log_fp = fopen(LOG_FILE, "a")) == NULL) {
        perror("fopen log error");
        close(c_fd);
        close(s_fd);
        exit(1);
    }

    // 6. loop: recv, write to file, echo back
    while (1) {
        n = recv(c_fd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            if (n == 0) printf("Client disconnected.\n");
            else        perror("recv error");
            break;
        }
        buf[n] = '\0';                  // ensure null-terminated
        printf("Received: %s", buf);

        // write to file
        fwrite(buf, 1, n, log_fp);
        fflush(log_fp);                 // flush so file is updated immediately

        // echo back
        if (send(c_fd, "OK", 2, 0) == -1) {
            perror("send error");
            break;
        }
    }

    // 7. cleanup
    fclose(log_fp);
    close(c_fd);
    close(s_fd);
    printf("Server exited.\n");
    return 0;
}
