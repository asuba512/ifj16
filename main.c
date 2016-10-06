/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include <stdio.h>

extern int token_error;

int main(){

	FILE *fd = fopen("zdroj.ifj","r");
	if(fd == NULL){
		printf("error\n");
		return 1;
	}
	
	token_t x = get_token(fd);
	
	while(token_error != EOF){
		if(x != NULL){ // <<< THIS IS ERROR no. 1;;;; what error is int x = 55c????
			switch(x->type){
				case token_double:
					printf("DOUBLE: %g\n", x->attr.d);
					break;
				case token_int:
					printf("INT: %d\n", x->attr.i);
					break;
				case token_string:
					printf("STRING: %s\n", x->attr.s->data);
					break;
				case token_id:
					printf("ID: %s\n", x->attr.s->data);
					break;
			}
		}
		x = get_token(fd);
	}

	fclose(fd);

    return 0;
}
