CFLAGS=-std=c99 -Wall -Wextra -pedantic -O2
CC=gcc
BIN=ifj
RM=rm -f

.phony: clean

all: scanner.o main.o infinite_string.o ial.o parser.o semantic_analysis.o sym_table.o token.o precedence.o ifj16_class.o interpret.o gc.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean: clean-obj
	$(RM) $(BIN) sc_test sem_test prec_test .fuse*

clean-obj:
	$(RM) *.o

doc:
	latex docs/doc.tex
	latex docs/doc.tex
	dvips doc.dvi 
	ps2pdf -sPAPERSIZE=a4 doc.ps
	mv doc.pdf dokumentace.pdf
	rm `ls | grep "doc[.]"`

