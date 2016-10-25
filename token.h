#ifndef TOKEN_INC
#define TOKEN_INC 1
/**
 * \file token.h
 * \brief Header file of module implementing tokens.
 *
 */

#include "infinite_string.h"

typedef enum token_type{
	token_multiplication, // * 0
	token_division, // / 1
	token_addition, // + 2
	token_substraction, // - 3
	token_less, // < 4
	token_more, // > 5
	token_lesseq, // <= 6
	token_moreeq, // >= 7
	token_equal, // == 8
	token_nequal, // != 9
	token_and, // && 10
	token_or, // || 11
	token_not, // ! 12
	token_double, // floating point number 13
	token_int, // integer number 14
	token_boolean, // boolean literal 15
	token_string, // string 16
	token_lbracket, // ( 17
	token_rbracket, // ) 18
	token_lbrace, // { 19
	token_rbrace, // } 20
	token_comma, // , 21
	token_id, // identifier 22
	token_assign, // = 23
	token_semicolon, // ; 24
	token_dot, // . 25
	token_k_boolean, // keyword boolean 26
	token_k_break, // keyword break 27
	token_k_class, // keyword  class 28
	token_k_continue, // keyword continue 29
	token_k_do, // keyword do 30
	token_k_double, // keyword double 31
	token_k_else, // keyword else 32
	token_k_for, // keyword for 33
	token_k_if, // keyword if 34
	token_k_int, // keyword int 35
	token_k_return, // keyword return 36
	token_k_string, // keyword String 37
	token_k_static, // keyword static 38
	token_k_void, // keyword void 39
	token_k_while, // keyword while 40
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
