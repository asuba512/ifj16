/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "parser.h"
#include "token.h"
#include "infinite_string.h"
#include "ial.h"
#include <stdio.h>
#include <stdlib.h>

extern string_t buff; // <- variable which has to be destroyed before exit, internal scanner variable
extern token_t t;
extern FILE *fd;
extern int lexerror;

int main(int argc, char **argv){
	fd = fopen(argv[1],"r");
	if(fd == NULL){
		printf("error\n");
		return 99;
	}

	t = malloc(sizeof(struct token));

	int retval = c_list();
	
	printf("retval: %d\n", lexerror == 1 ? 1 : retval);
	printf("current token: %d\n", t->type);
	int c;
	while((c = getc(fd)) != EOF){
		putchar(c);
	}
	
	str_destroy(buff);

	free(t);
	fclose(fd);
    return 0;
}
