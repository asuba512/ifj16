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
* Elements of auxiliary stack
*/
typedef struct prec_auxstack_element{
	prec_st_element *ptr;
	struct prec_auxstack_element *next;
} *prec_auxst_element;

/*
* Auxiliary stack
*/
typedef struct prec_s{
	prec_auxst_element top;
} prec_auxstack_t;

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

/*
* Initialization of auxiliary stack
*/
void prec_auxstack_init();

/*
* Pushes element to auxiliary stack
*/
int prec_auxstack_push(prec_st_element *);

/*
* Removes top element from auxiliary stack
*/
void prec_auxstack_pop();

/*
* Returns top element of auxiliary stack
*/
prec_st_element prec_auxstack_top();

/*
* 1 == auxiliary stack is empty
*/
int prec_auxstack_is_empty();

prec_auxstack_t auxstack; // global precedence stack
prec_stack_t stack; // global auxiliary stack

#endif
