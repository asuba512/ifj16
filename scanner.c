/**
 * \file scanner.c
 * \brief Lexical analyzer module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include <stdio.h>
#include <ctype.h>

int get_token(FILE *fd) {

	int c, i = 0;
	char buff[100]; // temporary
	
	t_state state = state_init;

	while(1){

		c = getc(fd);
		if(c == EOF)
			return EOF;

        switch(state){
            case state_init:
                if(isalpha(c) || c == '$' || c == '_'){
					buff[i++] = c;
                    state = state_identifier;
                }
				else if(c == '0');
				else if(isdigit(c)){
					buff[i++] = c;
					state = state_integer;
				}
					
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
				else{
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
				else{
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
				else{
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
        }

    }
	return 0;
}
