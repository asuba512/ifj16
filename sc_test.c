/**
 * \file sc_test.c
 * \brief Test module for scanner.
 *
 */

#include "scanner.h"
//#include "parser.h"
#include "token.h"
#include "infinite_string.h"
#include "parser.h"
//#include "ial.h"
#include <stdio.h>
#include <stdlib.h>

extern string_t buff; // <- variable which has to be destroyed before exit, internal scanner variable
extern token_t t;
extern FILE *fd;

int main(int argc, char **argv){

	/* this is an example of how scanner interface works */
	/* get_token() returns a value 1 if there was lexical error, 0 if succeded */
	/* first parameter is source file, second is allocated token_t var */
	/* list of token types are listed in token.h */
	/* token_string and token_identifier has attribute in form of infinite string .. */
	/* .. this string can be (duplicated AND destroyed) OR (pointer can be copied and later destroyed) .. */
	/* .. depends on semantic analysis implementation */
	FILE *fd;

	fd = fopen(argv[1],"r");
	if(fd == NULL){
		printf("error\n");
		return 99;
	}
	
	int retval;
	(retval = get_token(fd, &t));
	while(t.type!=token_eof){
		if(retval != 1){ // <<< THIS IS ERROR no. 1
			switch(t.type){
				case token_double:
					printf("DOUBLE: %f\n", t.attr.d);
					break;
				case token_int:
					printf("INT: %d\n", t.attr.i);
					break;
				case token_string:
					printf("STRING: %s\n", t.attr.s->data);
					str_destroy(t.attr.s);
					break;
				case token_id:
					printf("ID: %s\n", t.attr.s->data);
					str_destroy(t.attr.s);
					break;
				case token_fqid:
					printf("FQID: %s\n", t.attr.s->data);
					str_destroy(t.attr.s);
					break;
				case token_boolean:
					printf("%s\n", t.attr.b ? "true" : "false");
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
				case token_k_boolean:
					printf("boolean\n");
					break;
				case token_k_break:
					printf("break\n");
					break;
				case token_k_class:
					printf("class\n");
					break;
				case token_k_continue:
					printf("continue\n");
					break;
				case token_k_do:
					printf("do\n");
					break;
				case token_k_double:
					printf("double\n");
					break;
				case token_k_else:
					printf("else\n");
					break;
				case token_k_for:
					printf("for\n");
					break;
				case token_k_if:
					printf("if\n");
					break;
				case token_k_int:
					printf("int\n");
					break;
				case token_k_return:
					printf("return\n");
					break;
				case token_k_string:
					printf("String\n");
					break;
				case token_k_static:
					printf("static\n");
					break;
				case token_k_void:
					printf("void\n");
					break;
				case token_k_while:
					printf("while\n");
					break;
				case token_comma:
					printf(",\n");
					break;
				case token_not:
					printf("!\n");
					break;
				case token_or:
					printf("||\n");
					break;
				case token_and:
					printf("&&\n");
					break;
				case token_dot:
					printf(".\n");
					break;
				case token_eof:
					printf("eof\n");
					break;
			}
		}
		else{
		}
		(retval = get_token(fd, &t));
	}

	/* 
		variable --buff-- is an internal buffer of scanner, however it is safer to destroy it outside scanner, before program exits
		I managed to make it so parser doesn't have to initialize or empty it, but it HAS TO BE destroyed!!!
	*/
	str_destroy(buff);

	fclose(fd);
    return 0;
}
