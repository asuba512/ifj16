#ifndef TOKEN_INC
#define TOKEN_INC 1
/**
 * \file token.h
 * \brief Header file of module implementing tokens.
 *
 */

#include "infinite_string.h"

typedef enum token_type {
	token_addition, // + 1
	token_substraction, // - 2
	token_multiplication, // * 3
	token_division, // / 4
	token_less, // < 5
	token_more, // > 6
	token_lesseq, // <= 7
	token_moreeq, // >= 8
	token_equal, // == 9
	token_nequal, // != 10
	token_lbracket, // ( 11
	token_rbracket, // ) 12
	token_and, // && 13
	token_or, // || 14
	token_not, // ! 15
	token_id, // identifier 16
	token_eof, // 17
	token_double, // floating point number 18
	token_int, // integer number 19
	token_boolean, // boolean literal 20
	token_string, // string 21
	token_lbrace, // { 22
	token_rbrace, // } 23
	token_comma, // , 24
	token_assign, // = 25
	token_semicolon, // ; 26
	token_dot, // . 27
	token_k_boolean, // keyword boolean 28
	token_k_break, // keyword break 29
	token_k_class, // keyword  class 30
	token_k_continue, // keyword continue 31
	token_k_do, // keyword do 32
	token_k_double, // keyword double 33
	token_k_else, // keyword else 33
	token_k_for, // keyword for 34
	token_k_if, // keyword if 35
	token_k_int, // keyword int 36
	token_k_return, // keyword return 37
	token_k_string, // keyword String 38
	token_k_static, // keyword static 39
	token_k_void, // keyword void 40
	token_k_while, // keyword while 41
	token_nonterminal
} token_type;

typedef union attr {
	double d;
	int i;
	string_t s;
	int b;
	void *p; // pointer into table of symbols, precedence analysis
} attr_t;

typedef struct token {
	token_type type;
	attr_t attr;
} token_t;


typedef struct tok_que_node {
	struct token tok;
	struct tok_que_node *next;
} *tok_que_node_t;

typedef struct tok_que {
	tok_que_node_t head;
	tok_que_node_t tail;
} *tok_que_t;

tok_que_t tok_que_init();
int tok_enqueue(tok_que_t queue, struct token t);
token_t tok_remove_head(tok_que_t queue);
void tok_que_destroy(tok_que_t queue);
int tok_que_empty(tok_que_t queue);

#endif
