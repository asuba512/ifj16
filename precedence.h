#ifndef PRECEDENCE_H
#define PRECEDENCE_H
#include "token.h"
#include "sym_table.h"

/*
* Elements of pseudo-stack of tokens for precedence analysis
*/
typedef struct prec_stack_element{
	token_t data;
	struct prec_stack_element *next;
	struct prec_stack_element *prev;
} *prec_st_element;

/*
* Pseudo-stack of tokens for precedence analysis
*/
typedef struct prec_stack{
	prec_st_element top;
} prec_stack_t;

/*
* Precedence analysis
*/
int precedence(tok_que_t, op_t*);

/*
* Initialization of precedence stack
*/
void prec_stack_init();

/*
* Pushes element to precedence stack (the list is double-linked)
*/
int prec_stack_push(token_t);


int prec_stack_insert(prec_st_element *, token_t);

/*
* Removes top element of precedence stack
*/
void prec_stack_pop();

/*
* Returns top element of precedence stack
*/
prec_st_element *prec_stack_top();

/*
* 1 == precedence stack is empty
*/
int prec_stack_is_empty();

prec_stack_t stack; // global precedence stack
int count_of_stops; // counter of stops in stack

#endif
