#include "scanner.h"
#include "infinite_string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define addchar(c) do{ if(str_addchar(buff, c) == 99) { fprintf(stderr, "ERR: Internal error.\n"); return 99; }} while(0)

int get_token(FILE *fd, token_t *t) {
	int c; // var for storing input characters
	int o = 0; // for indexing string | storing octal number
	char octal[4]; //			  <---'
	char *endptr; // for string-to-number conversions
	t_state state = state_init;
	
	if(buff == NULL){ // buff is global variable, if it wasnt used yet, it is NULL
		buff = str_init("");
		if(buff == NULL){
			fprintf(stderr, "ERR: Internal error.\n");
			return 99;
		}
	}
	else
		str_empty(buff); // if it is a consecutive call of get_token, empty buffer
	
	while(1){
			c = getc(fd);
        	switch(state){
	            case state_init: // initial state
        	        if(c == '/') // division, comments
					state = _state_division;
				else if(c == '*'){ // multiplication
					t->type = token_multiplication;
					return 0;
				}
				else if(c == '+'){ // addition
					t->type = token_addition;
					return 0;
				}
				else if(c == '-'){ // substraction
					t->type = token_substraction;
					return 0;
				}
				else if(c == '<') // less, less equal
					state = state_less;
				else if(c == '>') // more, more equal
					state = state_more;
				else if(c == '=') // equal, assign
					state = state_eq;
				else if(c == '!') // negation, not equal
					state = state_neq;
				else if(c == ';'){ // semicolon
					t->type = token_semicolon;
					return 0;
				}
				else if(c == '('){ // left bracket
					t->type = token_lbracket;
					return 0;
				}
				else if(c == ')'){ // right bracket
					t->type = token_rbracket;
					return 0;
				}
				else if(c == '{'){ // left brace
					t->type = token_lbrace;
					return 0;
				}
				else if(c == '}'){ // right brace
					t->type = token_rbrace;
					return 0;
				}
				else if(c == ','){ // comma
					t->type = token_comma;
					return 0;
				}
				else if(c == '&') // and
					state = state_and;
				else if(c == '|') // or
					state = state_or;
				else if(isalpha(c) || c == '$' || c == '_'){ // identifier
					addchar(c);	
                    state = state_identifier;
                }
				else if(isdigit(c)){ // integer, double
					addchar(c);	
					state = state_integer;
				}
				else if(c == '"') // string
					state = state_string;
				else if(c == EOF){ // EOF
					t->type = token_eof;
					return 0;
				}
				else if(isspace(c)); // ignoring spaces
				else // anything else
					return 1;
                break;
			case _state_division:
				if(c == '/') // line comment
					state  = state_linecomment;
				else if(c == '*') // block comment
					state = _state_blockcomment;
				else{ // division
					ungetc(c, fd);
					t->type = token_division;
					return 0;
				}
				break;
			case state_linecomment:
				if(c == '\n' || c == EOF) // EOF - in case of file without newline before EOF
					state = state_init;
				break;
			case _state_blockcomment:
				if(c == '*') // potential end of block comment
					state = state_blockcomment;
				else if(c == EOF){ // EOF without ending block comment properly
					return 1;
				}
				break;
			case state_blockcomment:
				if(c == '/') // end of block comment
					state = state_init;
				else if(c == '*'); // still potential end of block comment
				else if(c == EOF){ // EOF without ending block comment properly
					ungetc(c, fd);
					return 1;
				}
				else // no longer potential end of block comment
					state = _state_blockcomment;
				break;
            case state_identifier:
                if(isalnum(c) || c == '$' || c == '_') // supported characters
					addchar(c);	
                else{
					if(c == '"') // there can't be '"' right after identifier (e.g. id"), it doens't make much sense
						return 1;
					// checking if identifier is not a keyword: 
					if(!check_keyword(t)){
						if(c == '.') // '.' can't follow after keyword
							return 1;
						ungetc(c,fd);
						return 0;
					}
					else if(c == '.'){ // fully qualified identifier
						addchar(c);
						t->attr.s = str_init(buff->data);
						if(t->attr.s == NULL){
							fprintf(stderr, "ERR: Internal error.\n");
							return 99;
						}
						str_empty(buff);
						state = _state_dot_fqidentifier;
					}
					else{ // idenfitier
						t->type = token_id;
						t->attr.s = str_init(buff->data); // storing a copy of string contaiting the name
						if(t->attr.s == NULL){
							fprintf(stderr, "ERR: Internal error.\n");
							return 99;
						}
						ungetc(c, fd);
						return 0;
					}
					// this is return with keyword token:
					//return 0;
		        }
				break;
			case _state_dot_fqidentifier:
				if(isalpha(c) || c == '$' || c == '_'){ // fully qualified idenfier, second part
					addchar(c);
					state = state_fqidentifier;
				}
				else{ // anything else is an error
					str_destroy(t->attr.s);
					ungetc(c, fd);
					return 1;
				}
				break;
			case state_fqidentifier:
				if(isalnum(c) || c == '$' || c == '_') // supported characters
					addchar(c);
				else{ // FQID
					if(c == '"' || c == '.') // right after FQID can't follow '"' or '.'
						return 1;
					if(!check_keyword(t)) // if second part is a keyword, it is an error
						return 1;
					t->type = token_fqid;
					str_cat(t->attr.s, buff); // add the second part of id
					ungetc(c, fd);
					return 0;
				}
				break;
			case state_less:
				if(c == '=') // less equal
					t->type = token_lesseq;
				else{ // less
					t->type = token_less;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_more:
				if(c == '=') // more equal
					t->type = token_moreeq;
				else{ // more
					t->type = token_more;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_eq: // equal
				if(c == '=')
					t->type = token_equal;
				else{ // assign
					t->type = token_assign;
					ungetc(c, fd);
				}
				return 0;
				break;
			case state_neq:
				if(c == '='){ // not equal
					t->type = token_nequal;
					return 0;
				}
				else{
					ungetc(c, fd); // negate
					t->type = token_not;
					return 0;
				}
			case state_and:
				if(c == '&'){ // and
					t->type = token_and;
					return 0;
				}
				else{ // there can't be only &
					ungetc(c, fd);
					return 1;
				}
				break;
			case state_or:
				if(c == '|'){ // or
					t->type = token_or;
					return 0;
				}
				else{ // there can't be only |
					ungetc(c, fd);
					return 1;
				}
				break;
			case state_integer:
				if(isdigit(c)) // integer
					addchar(c);	
				else if(c == '.'){ // double in form: X.X???
					addchar(c);	
					state = _state_double_point;
				}
				else if(c == 'e' || c == 'E'){ // double in form: Xe??
					addchar(c);	
					state = _state_double_e;
				}
				else{ // end of integer
					ungetc(c, fd);
					if(isalpha(c)) // there can't be a letter right after integer
						return 1;
					t->type = token_int;
					t->attr.i = strtol(buff->data, &endptr, 10); // convert string to integer
					return 0;
				}
				break;
			
			case _state_double_point:
				if(isdigit(c)){ // there must be a digit after decimal point
					addchar(c);	
					state = state_double;
				}
				else{
					ungetc(c, fd);
					return 1;
				}
				break;
			
			case state_double:
				if(isdigit(c)) // more digits
					addchar(c);	
				else if(c == 'e' || c == 'E'){ // double in form: X.Xe??
					addchar(c);	
					state = _state_double_e;
				}
				else{ // double in form: X.X
					ungetc(c, fd);
					if(isalpha(c) || c == '.') // there can't be a letter or '.' right after double
						return 1;
					t->type = token_double;
					t->attr.d = strtod(buff->data, &endptr); // convert string to double
					return 0;
				}
				break;
			case _state_double_e:
				if(isdigit(c)){ // double in form: X.Xe? or Xe?
					addchar(c);	
					state = state_double_e;
				}
				else if(c == '+' || c == '-'){ // double in form: X.Xe+/-? or Xe+/-?
					addchar(c);	
					state = _state_double_e_sign;
				}
				else{
					ungetc(c, fd);
					return 1;
				}
				break;
			case _state_double_e_sign:
				if(isdigit(c)){ // there must be at least one digit after sign in double
					addchar(c);	
					state = state_double_e;
				}
				else{
					ungetc(c, fd);
					return 1;
				}
				break;
			case state_double_e:
				if(isdigit(c)) // more digits
					addchar(c);	
				else{ // double in form: X.XeX or XeX or X.Xe+/-X or Xe+/-X
					ungetc(c, fd);
					if(isalpha(c) || c == '.') // there can't be a letter or '.' right after double
						return 1;
					t->type = token_double;
					t->attr.d = strtod(buff->data, &endptr);
					return 0;
				}				
				break;
			case state_string:
				if(c == '\\'){ // escape sequence in string
					state = _state_string_escape;
				}
				else if(c == '"'){ // end of string
					t->type = token_string;
					t->attr.s = str_init(buff->data);
					if(t->attr.s == NULL){
						fprintf(stderr, "ERR: Internal error.\n");
						return 99;
					}
					return 0;
				}
				else if(c == '\n' || c == EOF){ // unfinished string
					return 1;
				}
				else
					addchar(c); // adding characters to string
				break;
			case _state_string_escape:
				if(c == 't' || c == 'n' || c == '"' || c == '\\'){ // supported escape sequences
					switch(c){ // converting to actual character
						case 't':
							addchar('\t'); break;	
						case 'n':
							addchar('\n'); break;	
						case '"':
							addchar('\"'); break;	
						case '\\':
							addchar('\\'); break;	
					}
					state = state_string; // continue in string
				}
				else if(c >= '0' && c <= '3'){ // octal escape sequences, \001 - \337
					octal[o++] = c;
					state = _state_string_octalx;
				}
				else{ // error
					ungetc(c, fd);
					return 1;
				}
				break;
			case _state_string_octalx:
				if(c >= '0' && c <= '7'){ // octal escape sequence, \001 - \337
					octal[o++] = c;
					state = _state_string_octalxx;
				}
				else{ //error
					ungetc(c, fd);
					return 1;
				}
				break;
			case _state_string_octalxx: 
				if(c >= '0' && c <= '7'){ // octal escape sequence, \001 - \337
					octal[o++] = c;
					octal[o] = 0;
					if((c = strtol(octal, &endptr, 8)) == 0) // converting to integer, unsupported character \000 is an error
						return 1;
					addchar(c);
					o = 0; // resetting counter for storing octal digits
					state = state_string;
				}
				else{ // error
					ungetc(c, fd);
					return 1;
				}
				break;
        }

    }
	return 0;
}

int check_keyword(token_t *t){
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
	else if(!strcmp(buff->data, "true") || !strcmp(buff->data, "false")){
		t->type = token_boolean;
		t->attr.b = !strcmp(buff->data, "true") ? 1 : 0; // converting true/false to 1/0
	}
	else if(!strcmp(buff->data, "void")){
		t->type = token_k_void;
	}
	else if(!strcmp(buff->data, "while")){
		t->type = token_k_while;
	}
	else
		return 1;
	return 0;
}
