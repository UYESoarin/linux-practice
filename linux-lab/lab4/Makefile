# Makefile for Lab 4: concurrent streaming text system
CC      = gcc
CFLAGS  = -Wall -g
TARGETS = server client
HEADERS = common.h

.PHONY: all clean run setup

all: $(TARGETS)

server: server.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ server.c

client: client.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ client.c

# create logs directory
setup:
	mkdir -p logs

clean:
	rm -f $(TARGETS)
	rm -f logs/*.log

run: all setup
	@echo "Now open two or more terminals:"
	@echo "  Terminal 1 (server): ./server"
	@echo "  Terminal 2 (client): ./client 127.0.0.1 3339"
	@echo "  Terminal 3 (client): ./client 127.0.0.1 3339"
