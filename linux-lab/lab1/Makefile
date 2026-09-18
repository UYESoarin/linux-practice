# Makefile

# Configure Field (lab1 process management)
CC = gcc                # compiler
CFLAGS = -Wall -g       # warning + debug
TARGET = main child     # out files name
HEADERS = common.h      # pulic header file
MAIN_EXE = main         # run file

# Rule Field (Tab)
.PHONY: all clean run

all: $(TARGETS)         #default dest

%: %.c $(HEADERS)		# `make (xxxx)`
    $(CC) $(CFLAGS) $< -o $@

clean:					# `make clean`
    rm -f $(TARGETS)

run: all				# `make run`
    ./$(MAIN_EXE)
