/**
 * \file scanner.c
 * \brief Lexical analyzer module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern int token_error;

token_t get_token(FILE *fd) {

	int c, o = 0;
	string_t buff = str_init(""); 
	char octal[4];
	char *endptr;
	
	t_state state = state_init;

	token_t t;

	if((t = malloc(sizeof(struct token))) == NULL){
		token_error = 1;
		return NULL;
	}
	
	while(1){

		c = getc(fd);
		if(c == EOF){
			token_error = EOF;
			return NULL;
		}

        switch(state){
            case state_init:
                if(c == '/')
					state = _state_division;
                else if(isalpha(c) || c == '$' || c == '_'){
					str_addchar(buff, c);	
                    state = state_identifier;
                }
				else if(c == '0');
				else if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_integer;
				}
				else if(c == '"'){
					str_addchar(buff, c);	
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
					str_addchar(buff, c);	
                else{
					ungetc(c, fd);
					t->type = token_id;
					t->attr.s = buff;
					return t;
                }
                break;
			
			case state_integer:
				if(isdigit(c))
					str_addchar(buff, c);	
				else if(c == '.'){
					str_addchar(buff, c);	
					state = _state_double_point;
				}
				else if(c == 'e' || c == 'E'){
					str_addchar(buff, c);	
					state = _state_double_e;
				}
				else{
					ungetc(c, fd);
					t->type = token_int;
					t->attr.i = strtol(buff->data, &endptr, 10);
					return t;
				}
				break;
			
			case _state_double_point:
				if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_double;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
				break;
			
			case state_double:
				if(isdigit(c))
					str_addchar(buff, c);	
				else if(c == 'e' || c == 'E'){
					str_addchar(buff, c);	
					state = _state_double_e;
				}
				else{
					ungetc(c, fd);
					t->type = token_double;
					t->attr.d = strtod(buff->data, &endptr);
					return t;
				}
				break;
			case _state_double_e:
				if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_double_e;
				}
				else if(c == '+' || c == '-'){
					str_addchar(buff, c);	
					state = _state_double_e_sign;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
				break;
			case _state_double_e_sign:
				if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_double_e;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
				break;
			case state_double_e:
				if(isdigit(c))
					str_addchar(buff, c);	
				else{
					ungetc(c, fd);
					t->type = token_double;
					t->attr.d = strtod(buff->data, &endptr);
					return t;
				}				
				break;
			case state_string:
				if(c == '\\'){
					state = _state_string_escape;
				}
				else if(c == '"'){
					str_addchar(buff, c);	
					t->type = token_string;
					t->attr.s = buff;
					return t;
				}
				else
					str_addchar(buff, c);	
				break;
			case _state_string_escape:
				if(c == 't' || c == 'n' || c == '"' || c == '\\'){
					switch(c){
						case 't':
							str_addchar(buff, '\t'); break;	
						case 'n':
							str_addchar(buff, '\n'); break;	
						case '"':
							str_addchar(buff, '\"'); break;	
						case '\\':
							str_addchar(buff, '\\'); break;	
					}
					state = state_string;
				}
				else if(c >= '0' && c <= '3'){
					octal[o++] = c;
					state = _state_string_octalx;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
				break;
			case _state_string_octalx:
				if(c >= '0' && c <= '7'){
					octal[o++] = c;
					state = _state_string_octalxx;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
				break;
			case _state_string_octalxx:
				if(c >= '0' && c <= '7'){
					octal[o++] = c;
					octal[o] = 0;
					str_addchar(buff, strtol(octal, &endptr, 8));
					o = 0;
					state = state_string;
				}
				else{ // <- future error state
					ungetc(c, fd);
					token_error = 1; //TODO
					free(t);
					return NULL;
				}
        }

    }
	return 0;
}
