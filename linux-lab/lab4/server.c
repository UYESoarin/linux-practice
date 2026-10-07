/* server.c - concurrent streaming text server
   fork per client, stream corpus line char by char
   run: ./server
*/
#include "common.h"
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_LINES     200
#define MAX_LINE_LEN  512
#define CHAR_DELAY_US 80000     // 80ms per char = typewriter effect

static char g_corpus[MAX_LINES][MAX_LINE_LEN];
static int  g_corpus_count = 0;

// load corpus(file) return void, fill g_corpus
static void load_corpus(void) {
    FILE *fp = fopen(CORPUS_FILE, "r");
    if (fp == NULL) {
        perror("fopen corpus error");
        exit(1);
    }
    while (g_corpus_count < MAX_LINES &&
           fgets(g_corpus[g_corpus_count], MAX_LINE_LEN, fp) != NULL) {
        // strip trailing newline
        g_corpus[g_corpus_count][strcspn(g_corpus[g_corpus_count], "\n")] = '\0';
        if (strlen(g_corpus[g_corpus_count]) > 0) {
            g_corpus_count++;
        }
    }
    fclose(fp);
    printf("[Server] loaded %d corpus lines\n", g_corpus_count);
}

// SIGCHLD handler: reap zombie children without blocking
static void sigchld_handler(int signo) {
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// stream one line to client(c_fd, line) char by char
static int stream_line(int c_fd, const char *line) {
    int len = strlen(line);
    for (int i = 0; i < len; i++) {
        if (send_all(c_fd, &line[i], 1) == -1) return -1;
        usleep(CHAR_DELAY_US);
    }
    // send newline to mark end of line
    return send_all(c_fd, "\n", 1);
}

// handle one client(c_fd, client_id): loop recv, random reply, log
static void handle_client(int c_fd, int client_id) {
    char  buf[BUF_SIZE];
    char  log_path[128];
    FILE *log_fp = NULL;
    int   n;

    // open per-client log file
    snprintf(log_path, sizeof(log_path), "%s/client_%d.log", LOG_DIR, client_id);
    log_fp = fopen(log_path, "w");
    if (log_fp == NULL) {
        perror("fopen log error");
        // continue without log
    }

    // seed random: time + pid to ensure different per child
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    printf("[Server] child %d serving client %d\n", getpid(), client_id);

    // send welcome
    const char *welcome = "Welcome to streaming server! Type a message:\n";
    send_all(c_fd, welcome, strlen(welcome));

    while (1) {
        n = recv_line(c_fd, buf, BUF_SIZE);
        if (n <= 0) {
            if (n == 0) printf("[Server] client %d disconnected\n", client_id);
            else        perror("recv error");
            break;
        }

        // strip newline
        buf[strcspn(buf, "\n")] = '\0';

        // strict quit check
        if (strcmp(buf, "quit") == 0) {
            send_all(c_fd, "Bye!\n", 5);
            break;
        }

        printf("[Server] client %d said: %s\n", client_id, buf);

        // log user input
        if (log_fp) {
            fprintf(log_fp, "[User] %s\n", buf);
            fflush(log_fp);
        }

        // pick random corpus line
        if (g_corpus_count > 0) {
            int idx = rand() % g_corpus_count;
            const char *line = g_corpus[idx];

            // stream to client
            if (stream_line(c_fd, line) == -1) break;

            // log server response
            if (log_fp) {
                fprintf(log_fp, "[Server] %s\n", line);
                fflush(log_fp);
            }
        }
    }

    if (log_fp) fclose(log_fp);
    close(c_fd);
    printf("[Server] child %d exit\n", getpid());
    exit(0);
}

int main(void) {
    int                s_fd, c_fd;
    struct sockaddr_in s_addr, c_addr;
    socklen_t          c_len;
    int                opt = 1;
    int                client_count = 0;

    // load corpus before any client
    load_corpus();

    // ensure log dir exists (user must create manually with: mkdir -p logs)
    // if not exist, log files will fail silently

    // reap zombie children automatically
    signal(SIGCHLD, sigchld_handler);

    // create socket
    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    // set port reuse
    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt error");
        exit(1);
    }

    // bind
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port        = htons(PORT);
    if (bind(s_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        perror("bind error");
        exit(1);
    }

    // listen
    if (listen(s_fd, 10) == -1) {
        perror("listen error");
        exit(1);
    }
    printf("[Server] listening on port %d (pid=%d)\n", PORT, getpid());

    // main loop: accept + fork
    while (1) {
        c_len = sizeof(c_addr);
        if ((c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &c_len)) == -1) {
            perror("accept error");
            continue;
        }
        client_count++;

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork error");
            close(c_fd);
            continue;
        }
        if (pid == 0) {
            // child: handle this client, close listen fd
            close(s_fd);
            handle_client(c_fd, client_count);
            // handle_client ends with exit(0)
        } else {
            // parent: close connect fd, loop to accept next
            close(c_fd);
            printf("[Server] forked child %d for client %d\n", pid, client_count);
        }
    }

    close(s_fd);
    return 0;
}
