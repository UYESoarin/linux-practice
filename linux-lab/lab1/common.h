/* common.h — 公共头文件 */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>    // 标准输入输出
#include <stdlib.h>   // 通用工具、exit
#include <unistd.h>   // UNIX系统调用：fork、execl、sleep、getpid
#include <signal.h>   // 信号相关：sigaction、signal、kill、sigemptyset
#include <sys/types.h>// 系统类型定义：pid_t
#include <sys/wait.h> // waitpid，以及WIFEXITED/WEXITSTATUS等宏
#include <errno.h>    // errno 全局错误变量，EINTR

#endif
