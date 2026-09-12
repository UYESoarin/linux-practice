/* main.c — 实验一主程序
 * 功能：演示fork创建进程、exec执行新程序、sigaction处理信号
 * 流程：父进程注册SIGINT → fork子进程 → 子进程exec执行child
 *       父进程wait等待 → 用户按Ctrl+C → 父进程发SIGTERM给子进程
 *       子进程退出 → 父进程回收并退出
 */
#include "common.h"

pid_t g_child_pid = -1;   /* 全局变量保存子进程PID，供信号处理函数使用 */

/* SIGINT信号处理函数：父进程收到Ctrl+C时执行 */
void sigint_handler(int signo)
{
    printf("\n[Parent %d] Received SIGINT (signal %d)\n", getpid(), signo);

    if (g_child_pid > 0) {
        printf("[Parent %d] Sending SIGTERM to child %d\n",
               getpid(), g_child_pid);
        kill(g_child_pid, SIGTERM);   /* 向子进程发送SIGTERM */
    }
}

int main(void)
{
    /* ====== 1. 使用sigaction注册SIGINT信号处理函数 ====== */
    struct sigaction act;
    act.sa_handler = sigint_handler;    /* 指定处理函数 */
    sigemptyset(&act.sa_mask);          /* 清空信号屏蔽集 */
    act.sa_flags = 0;                   /* 默认标志 */

    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    printf("[Parent %d] SIGINT handler registered.\n", getpid());

    /* ====== 2. 使用fork创建子进程 ====== */
    g_child_pid = fork();

    if (g_child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (g_child_pid == 0) {
        /* ====== 3. 子进程：使用exec执行新程序 ====== */
        printf("[Child  %d] About to exec ./child\n", getpid());

        /* execl执行当前目录下的child程序，替换子进程映像 */
        execl("./child", "child", NULL);

        /* 只有execl失败才会执行到这里 */
        perror("execl");
        exit(EXIT_FAILURE);
    }

    /* ====== 4. 父进程：等待子进程 ====== */
    printf("[Parent %d] Created child PID = %d\n", getpid(), g_child_pid);
    printf("[Parent %d] Press Ctrl+C to terminate child...\n", getpid());

    int status;
    /* waitpid被信号中断时返回-1且errno==EINTR，需要循环重试 */
    while (waitpid(g_child_pid, &status, 0) == -1) {
        if (errno == EINTR) {
            continue;   /* 被信号中断，继续等待 */
        }
        perror("waitpid");
        break;
    }

    /* 分析子进程退出状态 */
    if (WIFEXITED(status)) {
        printf("[Parent %d] Child exited normally, code = %d\n",
               getpid(), WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[Parent %d] Child killed by signal %d\n",
               getpid(), WTERMSIG(status));
    }

    printf("[Parent %d] Exiting.\n", getpid());
    return 0;
}
