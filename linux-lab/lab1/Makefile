# Makefile — 实验一进程管理
# 注意：命令行前必须是Tab键，不能是空格

CC = gcc
CFLAGS = -Wall -g
TARGETS = main child

# 默认目标：编译所有程序
all: $(TARGETS)

# 编译main程序
main: main.c common.h
	$(CC) $(CFLAGS) -o main main.c

# 编译child程序
child: child.c common.h
	$(CC) $(CFLAGS) -o child child.c

# 清理生成的文件
clean:
	rm -f $(TARGETS)

# 编译并运行
run: all
	./main

# 声明伪目标（不对应实际文件）
.PHONY: all clean run
