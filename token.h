#ifndef TOKEN_INC
#define TOKEN_INC 1
/**
 * \file token.h
 * \brief Header file of module implementing tokens.
 *
 */

#include "infinite_string.h"

typedef enum token_type{
	token_multiplication, // *
	token_division, // /
	token_addition, // + 
	token_substraction, // -
	token_less, // <
	token_more, // >
	token_lesseq, // <=
	token_moreeq, // >=
	token_equal, // ==
	token_nequal, // !=
	token_and, // &&
	token_or, // ||
	token_not, // !
	token_double, // floating point number
	token_int, // integer number
	token_boolean, // boolean literal
	token_string, // string
	token_lbracket, // (
	token_rbracket, // )
	token_lbrace, // {
	token_rbrace, // }
	token_comma, // ,
	token_id, // identifier
	token_assign, // =
	token_semicolon, // ;
	token_dot, // .
	token_k_boolean, // keyword boolean
	token_k_break, // keyword break
	token_k_class, // keyword  class
	token_k_continue, // keyword continue
	token_k_do, // keyword do
	token_k_double, // keyword double
	token_k_else, // keyword else
	token_k_for, // keyword for
	token_k_if, // keyword if
	token_k_int, // keyword int
	token_k_return, // keyword return
	token_k_string, // keyword String
	token_k_static, // keyword static
	token_k_void, // keyword void
	token_k_while, // keyword while
	token_eof
} token_type;

typedef union attr{
	double d;
	int i;
	string_t s;
	int b;
} attr_t;

typedef struct token{
	token_type type;
	attr_t attr;
} *token_t;

#endif
