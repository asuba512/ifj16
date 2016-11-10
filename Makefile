CFLAGS=-std=c11 -Wall -Wextra -pedantic -g
CC=gcc

BIN=ifj

RM=rm -f

.phony: clean

all: scanner.o main.o infinite_string.o ial.o parser.o semantic_analysis.o sym_table.o token.o precedence.o
	$(CC) $(CFLAGS) -o $(BIN) $^

test: scanner.o sc_test.o infinite_string.o
	$(CC) $(CFLAGS) -o sc_test $^
	make clean-obj

sem_test: infinite_string.o ial.o parser.o semantic_analysis.o sym_table.o scanner.o sem_test.o token.o
	$(CC) $(CFLAGS) -o sem_test $^
	make clean-obj

prec_test: prec_test.o infinite_string.o token.o precedence.o
	$(CC) $(CFLAGS) -o prec_test $^
	make clean-obj

clean: clean-obj
	$(RM) $(BIN) sc_test sem_test prec_test

clean-obj:
	$(RM) *.o
