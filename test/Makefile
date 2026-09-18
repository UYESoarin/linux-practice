CC = gcc
CFLAGS = -Wall -g
TARGETS = hello
HEADERS = 
MAIN_EXE = hello

.PHONY: all clean run

all: $(TARGETS)

%: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

run:
	./$(MAIN_EXE)
