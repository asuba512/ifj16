CFLAGS=-std=c99 -Wall -Wextra -pedantic -g
CC=gcc

BIN=ifj

RM=rm -f

.phony: clean

all: scanner.o main.o infinite_string.o ial.o parser.o
	$(CC) $(CFLAGS) -o $(BIN) $^

test: scanner.o sc_test.o infinite_string.o ial.o parser.o
	$(CC) $(CFLAGS) -o sc_test $^
	make clean-obj

clean: clean-obj
	$(RM) $(BIN) sc_test

clean-obj:
	$(RM) *.o
