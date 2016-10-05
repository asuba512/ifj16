CFLAGS=-std=c99 -Wall -Wextra -pedantic -g
CC=gcc

BIN=ifj

RM=rm -f

.phony: clean

all: scanner.o main.o infinite_string.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	$(RM) *.o $(BIN)
