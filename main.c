/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include "ial.h"
#include <stdio.h>

extern int token_error;

string_t keywords[17];

int main(){

	FILE *fd = fopen("zdroj.ifj","r");
	if(fd == NULL){
		printf("error\n");
		return 1;
	}

	/*--------- temporary ------ */

	keywords[0] = str_init("boolean");
	keywords[1] = str_init("break");
	keywords[2] = str_init("class");
	keywords[3] = str_init("continue");
	keywords[4] = str_init("do");
	keywords[5] = str_init("double");
	keywords[6] = str_init("else");
	keywords[7] = str_init("false");
	keywords[8] = str_init("for");
	keywords[9] = str_init("if");
	keywords[10] = str_init("int");
	keywords[11] = str_init("return");
	keywords[12] = str_init("String");
	keywords[13] = str_init("static");
	keywords[14] = str_init("true");
	keywords[15] = str_init("void");
	keywords[16] = str_init("while");

	/* -------------- */

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
				case token_keyword:
					printf("KEYWORD: %s\n", x->attr.s->data);
					break;
			}
		}
		x = get_token(fd);
	}

	fclose(fd);
	for(int i = 0; i < 17; i++)
		str_destroy(keywords[i]);

    return 0;
}
