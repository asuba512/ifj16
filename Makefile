CFLAGS=-std=c99 -Wall -Wextra -pedantic
CC=gcc

BIN=ifj

RM=rm -f

.phony: clean

all: scanner.o main.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	$(RM) *.o $(BIN)
