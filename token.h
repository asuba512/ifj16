#ifndef TOKEN_INC
#define TOKEN_INC 1

#include "infinite_string.h"

typedef enum token_type {
	token_addition, // + 0
	token_substraction, // - 1
	token_multiplication, // * 2
	token_division, // / 3
	token_less, // < 4
	token_more, // > 5
	token_lesseq, // <= 6
	token_moreeq, // >= 7
	token_equal, // == 8
	token_nequal, // != 9
	token_lbracket, // ( 10
	token_rbracket, // ) 11
	token_and, // && 12
	token_or, // || 13
	token_not, // ! 14
	token_id, // identifier 15
	token_eof, // 16
	token_fqid, // fully qualified identifier 17
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
	token_k_else, // keyword else 34
	token_k_for, // keyword for 35
	token_k_if, // keyword if 36
	token_k_int, // keyword int 37
	token_k_return, // keyword return 38
	token_k_string, // keyword String 39
	token_k_static, // keyword static 40
	token_k_void, // keyword void 41
	token_k_while, // keyword while 42
	token_nonterminal // for precedence analysis 43
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
