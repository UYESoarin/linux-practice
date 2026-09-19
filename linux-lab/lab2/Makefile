# Makefile for Lab 2: shared memory + semaphore IPC
CC      = gcc
CFLAGS  = -Wall -g
TARGETS = semshm-write semshm-read
OBJS    = semshm.o
HEADERS = semshm.h

.PHONY: all clean run

all: $(TARGETS)

# compile tool c
semshm.o: semshm.c $(HEADERS)
	$(CC) $(CFLAGS) -c semshm.c

# link write process
semshm-write: semshm-write.c semshm.o $(HEADERS)
	$(CC) $(CFLAGS) -o $@ semshm-write.c semshm.o

# link read process
semshm-read: semshm-read.c semshm.o $(HEADERS)
	$(CC) $(CFLAGS) -o $@ semshm-read.c semshm.o

clean:
	rm -f $(TARGETS) $(OBJS)

run:
	@echo "Open two terminals and run:"
	@echo "  Terminal 1 (Server): ./semshm-write"
	@echo "  Terminal 2 (Client): ./semshm-read"
