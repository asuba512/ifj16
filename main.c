/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include "ial.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

extern int token_error;

string_t keywords[17];

int main(){

	FILE *fd = fopen("zdroj.ifj","r");
	if(fd == NULL){
		printf("error\n");
		return 1;
	}

	token_t x = malloc(sizeof(struct token));
	int retval;	

	while((retval = get_token(fd, x)) != EOF){
		if(retval != 1){ // <<< THIS IS ERROR no. 1
			switch(x->type){
				case token_double:
					printf("DOUBLE: %g\n", x->attr.d);
					break;
				case token_int:
					printf("INT: %d\n", x->attr.i);
					break;
				case token_string:
					printf("STRING: %s\n", x->attr.s->data);
					str_destroy(x->attr.s);
					break;
				case token_id:
					printf("ID: %s\n", x->attr.s->data);
					str_destroy(x->attr.s);
					break;
				case token_division:
					printf("/\n");
					break;
				case token_multiplication:
					printf("*\n");
					break;
				case token_addition:
					printf("+\n");
					break;
				case token_substraction:
					printf("-\n");
					break;
				case token_less:
					printf("<\n");
					break;
				case token_more:
					printf(">\n");
					break;
				case token_lesseq:
					printf("<=\n");
					break;
				case token_moreeq:
					printf(">=\n");
					break;
				case token_equal:
					printf("==\n");
					break;
				case token_nequal:
					printf("!=\n");
					break;
				case token_semicolon:
					printf(";\n");
					break;
				case token_assign:
					printf("=\n");
					break;
				case token_lbracket:
					printf("(\n");
					break;
				case token_rbracket:
					printf(")\n");
					break;
				case token_lbrace:
					printf("{\n");
					break;
				case token_rbrace:
					printf("}\n");
					break;
			}
		}
	}

	fclose(fd);
    return 0;
}
