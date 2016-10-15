#ifndef TOKEN_INC
#define TOKEN_INC 1
/**
 * \file token.h
 * \brief Header file of module implementing tokens.
 *
 */

#include "infinite_string.h"

typedef enum token_type{
	token_multiplication,
	token_division,
	token_addition,
	token_substraction,
	token_less,
	token_more,
	token_lesseq,
	token_moreeq,
	token_equal,
	token_nequal,
	token_double,
	token_int,
	token_string,
	token_lbracket,
	token_rbracket,
	token_lbrace,
	token_rbrace,
	token_id,
	token_assign,
	token_semicolon,
	token_k_boolean,
	token_k_break,
	token_k_class,
	token_k_continue,
	token_k_do,
	token_k_double,
	token_k_else,
	token_k_false,
	token_k_for,
	token_k_if,
	token_k_int,
	token_k_return,
	token_k_string,
	token_k_static,
	token_k_true,
	token_k_void,
	token_k_while
} token_type;

typedef union attr{
	double d;
	int i;
	string_t s;
} attr_t;

typedef struct token{
	token_type type;
	attr_t attr;
} *token_t;

#endif
