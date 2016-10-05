/**
 * \file scanner.c
 * \brief Lexical analyzer module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int get_token(FILE *fd) {

	int c, i = 0, o = 0;
	char buff[100]; // temporary
	char octal[4];
	char *endptr;
	
	t_state state = state_init;

	while(1){

		c = getc(fd);
		if(c == EOF)
			return EOF;

        switch(state){
            case state_init:
                if(c == '/')
					state = _state_division;
                else if(isalpha(c) || c == '$' || c == '_'){
					buff[i++] = c;
                    state = state_identifier;
                }
				else if(c == '0');
				else if(isdigit(c)){
					buff[i++] = c;
					state = state_integer;
				}
				else if(c == '"'){
					buff[i++] = c;
					state = state_string;
				}
					
                break;
			case _state_division:
				if(c == '/')
					state  = state_linecomment;
				else if(c == '*')
					state = _state_blockcomment;
				else{ // <- future division sign return
					ungetc(c, fd);
					state = state_init;
				}
				break;
			case state_linecomment:
				if(c == '\n')
					state = state_init;
				break;
			case _state_blockcomment:
				if(c == '*')
					state = state_blockcomment;
				break;
			case state_blockcomment:
				if(c == '/')
					state = state_init;
				else
					state = _state_blockcomment;
				break;
            case state_identifier:
                if(isalnum(c) || c == '$' || c == '_')
					buff[i++] = c;
                else{
					ungetc(c, fd);
					buff[i] = 0;
                    printf("ID: %s\n",buff);
					i = 0;
                    state = state_init;
                }
                break;
			
			case state_integer:
				if(isdigit(c))
					buff[i++] = c;
				else if(c == '.'){
					buff[i++] = c;
					state = _state_double_point;
				}
				else if(c == 'e' || c == 'E'){
					buff[i++] = c;
					state = _state_double_e;
				}
				else{
					ungetc(c, fd);
					buff[i] = 0;
					printf("INTEGER: %s\n", buff);
					i = 0;
					state = state_init;
				}
				break;
			
			case _state_double_point:
				if(isdigit(c)){
					buff[i++] = c;
					state = state_double;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
				break;
			
			case state_double:
				if(isdigit(c))
					buff[i++] = c;
				else if(c == 'e' || c == 'E'){
					buff[i++] = c;
					state = _state_double_e;
				}
				else{
					ungetc(c, fd);
					buff[i] = 0;
					printf("REAL: %s\n", buff);
					i = 0;
					state = state_init;
				}
				break;
			case _state_double_e:
				if(isdigit(c)){
					buff[i++] = c;
					state = state_double_e;
				}
				else if(c == '+' || c == '-'){
					buff[i++] = c;
					state = _state_double_e_sign;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
				break;
			case _state_double_e_sign:
				if(isdigit(c)){
					buff[i++] = c;
					state = state_double_e;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
				break;
			case state_double_e:
				if(isdigit(c))
					buff[i++] = c;
				else{
					ungetc(c, fd);
					buff[i] = 0;
					printf("REAL: %s\n", buff);
					i = 0;
					state = state_init;
				}				
				break;
			case state_string:
				if(c == '\\'){
					state = _state_string_escape;
				}
				else if(c == '"'){
					buff[i++] = c;
					buff[i] = 0;
					printf("STRING: %s\n", buff);
					i = 0;
					state = state_init;
				}
				else
					buff[i++] = c;
				break;
			case _state_string_escape:
				if(c == 't' || c == 'n' || c == '"' || c == '\\'){
					switch(c){
						case 't':
							buff[i++] = '\t'; break;
						case 'n':
							buff[i++] = '\n'; break;
						case '"':
							buff[i++] = '\"'; break;
						case '\\':
							buff[i++] = '\\'; break;
					}
					state = state_string;
				}
				else if(c >= '0' && c <= '3'){
					octal[o++] = c;
					state = _state_string_octalx;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
				break;
			case _state_string_octalx:
				if(c >= '0' && c <= '7'){
					octal[o++] = c;
					state = _state_string_octalxx;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
				break;
			case _state_string_octalxx:
				if(c >= '0' && c <= '7'){
					octal[o++] = c;
					octal[o] = 0;
					buff[i++] = strtol(octal, &endptr, 8);
					o = 0;
					state = state_string;
				}
				else{ // <- future error state
					ungetc(c, fd);
					i = 0;
					state = state_init;
				}
        }

    }
	return 0;
}
