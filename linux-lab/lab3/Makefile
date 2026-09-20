# Makefile for Lab 3: TCP + file I/O
CC      = gcc
CFLAGS  = -Wall -g
TARGETS = tcp-server tcp-client
HEADERS = tcp-common.h

.PHONY: all clean run

all: $(TARGETS)

tcp-server: tcp-server.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ tcp-server.c

tcp-client: tcp-client.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ tcp-client.c

clean:
	rm -f $(TARGETS) messages.log

run:
	@echo "Open two terminals:"
	@echo "  Terminal 1: ./tcp-server"
	@echo "  Terminal 2: ./tcp-client 127.0.0.1 3339"
