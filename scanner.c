/**
 * \file scanner.c
 * \brief Lexical analyzer module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int get_token(FILE *fd, token_t t) {
	int c; // var for storing input characters
	int o = 0; // for indexing c-string | storing octal number
	char octal[4]; //			    <---'
	char *endptr; // for string-to-number conversions
	t_state state = state_init;
	
	if(buff == NULL) // buff is global variable, if it wasnt used yet, it is NULL
		buff = str_init("");
	else
		str_empty(buff); // if it is a consecutive call of get_token, empty buffer
	
	while(1){
		c = getc(fd);		
		if(c == EOF){
			t->type = token_eof;
			return EOF;
		}

        switch(state){
            case state_init:
                if(c == '/')
					state = _state_division;
				else if(c == '*'){
					t->type = token_multiplication;
					return 0;
				}
				else if(c == '+'){
					t->type = token_addition;
					return 0;
				}
				else if(c == '-'){
					t->type = token_substraction;
					return 0;
				}
				else if(c == '<')
					state = state_less;
				else if(c == '>')
					state = state_more;
				else if(c == '=')
					state = state_eq;
				else if(c == '!')
					state = state_neq;
				else if(c == ';'){
					t->type = token_semicolon;
					return 0;
				}
				else if(c == '('){
					t->type = token_lbracket;
					return 0;
				}
				else if(c == ')'){
					t->type = token_rbracket;
					return 0;
				}
				else if(c == '{'){
					t->type = token_lbrace;
					return 0;
				}
				else if(c == '}'){
					t->type = token_rbrace;
					return 0;
				}
				else if(c == ','){
					t->type = token_comma;
					return 0;
				}
				else if(c == '.'){
					t->type = token_dot;
					return 0;
				}
				else if(c == '&')
					state = state_and;
				else if(c == '|')
					state = state_or;
                else if(isalpha(c) || c == '$' || c == '_'){
					str_addchar(buff, c);	
                    state = state_identifier;
                }
				else if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_integer;
				}
				else if(c == '"'){
					str_addchar(buff, c);	
					state = state_string;
				}
				else if(isspace(c));
				else{
					return 1;
				}
                break;
			case _state_division:
				if(c == '/')
					state  = state_linecomment;
				else if(c == '*')
					state = _state_blockcomment;
				else{
					ungetc(c, fd);
					t->type = token_division;
					return 0;
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
					if(!strcmp(buff->data, "boolean")){
						t->type = token_k_boolean;
					}
					else if(!strcmp(buff->data, "break")){
						t->type = token_k_break;
					}
					else if(!strcmp(buff->data, "class")){
						t->type = token_k_class;
					}
					else if(!strcmp(buff->data, "continue")){
						t->type = token_k_continue;
					}
					else if(!strcmp(buff->data, "do")){
						t->type = token_k_do;
					}
					else if(!strcmp(buff->data, "double")){
						t->type = token_k_double;
					}
					else if(!strcmp(buff->data, "else")){
						t->type = token_k_else;
					}
					else if(!strcmp(buff->data, "false")){
						t->type = token_k_false;
					}
					else if(!strcmp(buff->data, "for")){
						t->type = token_k_for;
					}
					else if(!strcmp(buff->data, "if")){
						t->type = token_k_if;
					}
					else if(!strcmp(buff->data, "int")){
						t->type = token_k_int;
					}
					else if(!strcmp(buff->data, "return")){
						t->type = token_k_return;
					}
					else if(!strcmp(buff->data, "String")){
						t->type = token_k_string;
					}
					else if(!strcmp(buff->data, "static")){
						t->type = token_k_static;
					}
					else if(!strcmp(buff->data, "true")){
						t->type = token_k_true;
					}
					else if(!strcmp(buff->data, "void")){
						t->type = token_k_void;
					}
					else if(!strcmp(buff->data, "while")){
						t->type = token_k_while;
					}
					else{
						t->type = token_id;
						t->attr.s = str_init(buff->data);
						return 0;
					}
					return 0;
                }
                break;
			case state_less:
				if(c == '=')
					t->type = token_lesseq;
				else{
					t->type = token_less;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_more:
				if(c == '=')
					t->type = token_moreeq;
				else{
					t->type = token_more;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_eq:
				if(c == '=')
					t->type = token_equal;
				else{
					t->type = token_assign;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_neq:
				if(c == '='){
					t->type = token_nequal;
					return 0;
				}
				else{
					ungetc(c, fd);
					t->type = token_not;
					return 0;
				}
			case state_and:
				if(c == '&'){
					t->type = token_and;
					return 0;
				}
				else{
					return 1;
				}
				break;
			case state_or:
				if(c == '|'){
					t->type = token_or;
					return 0;
				}
				else{
					return 1;
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
				else if(isalpha(c)){
					t = NULL;;
					return 1;
				}
				else{
					 ungetc(c, fd);
					t->type = token_int;
					t->attr.i = strtol(buff->data, &endptr, 10);
					return 0;
				}
				break;
			
			case _state_double_point:
				if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_double;
				}
				else{
					ungetc(c, fd); 
					t = NULL;;
					return 1;
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
					return 0;
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
				else{
					ungetc(c, fd);
					t = NULL;;
					return 1;
				}
				break;
			case _state_double_e_sign:
				if(isdigit(c)){
					str_addchar(buff, c);	
					state = state_double_e;
				}
				else{
					ungetc(c, fd);
					t = NULL;;
					return 1;
				}
				break;
			case state_double_e:
				if(isdigit(c))
					str_addchar(buff, c);	
				else{
					ungetc(c, fd);
					t->type = token_double;
					t->attr.d = strtod(buff->data, &endptr);
					return 0;
				}				
				break;
			case state_string:
				if(c == '\\'){
					state = _state_string_escape;
				}
				else if(c == '"'){
					str_addchar(buff, c);	
					t->type = token_string;
					t->attr.s = str_init(buff->data);
					return 0;
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
				else{
					ungetc(c, fd);
					t = NULL;;
					return 1;
				}
				break;
			case _state_string_octalx:
				if(c >= '0' && c <= '7'){
					octal[o++] = c;
					state = _state_string_octalxx;
				}
				else{
					ungetc(c, fd);
					t = NULL;;
					return 1;
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
				else{
					ungetc(c, fd);
					t = NULL;;
					return 1;
				}
				break;
        }

    }
	return 0;
}

