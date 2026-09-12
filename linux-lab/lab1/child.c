/* child.c — 子进程exec后执行的目标程序
 * 功能：循环输出信息，收到SIGTERM后优雅退出
 * 演示：signal注册信号处理函数、进程被信号终止
 */
#include "common.h"

volatile sig_atomic_t g_running = 1;   /* 运行标志，sig_atomic_t保证原子性 */

/* SIGTERM信号处理函数：收到父进程发来的终止信号时执行 */
void sigterm_handler(int signo)
{
    printf("[Child  %d] Received SIGTERM (signal %d), exiting...\n",
           getpid(), signo);
    g_running = 0;   /* 修改标志，让主循环退出 */
}

int main(void)
{
    /* ====== 使用signal注册SIGTERM信号处理函数 ====== */
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("[Child  %d] Started. PID = %d\n", getpid(), getpid());
    printf("[Child  %d] Working... (send SIGTERM to stop)\n", getpid());

    int i = 0;
    while (g_running) {
        printf("[Child  %d] Working... %d\n", getpid(), i++);
        sleep(1);   /* 每秒输出一次 */
    }

    printf("[Child  %d] Exiting gracefully.\n", getpid());
    return 0;
}
