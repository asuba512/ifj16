/**
 * \file token.h
 * \brief Header file of module implementing tokens.
 *
 */

#include "infinite_string.h"

int token_error;

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
	token_id,
	token_keyword
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
