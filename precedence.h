#ifndef PRECEDENCE_H
#define PRECEDENCE_H
#include "token.h"
#include "sym_table.h"

typedef struct prec_stack_element{
	token_t data;
	struct prec_stack_element *next;
	struct prec_stack_element *prev;
} *prec_st_element;

typedef struct prec_stack{
	prec_st_element top;
} prec_stack_t;

typedef struct prec_auxstack_element{
	prec_st_element *ptr;
	struct prec_auxstack_element *next;
} *prec_auxst_element;

typedef struct prec_s{
	prec_auxst_element top;
} prec_auxstack_t;

int precedence(tok_que_t, op_t*);

void prec_stack_init();
int prec_stack_push(token_t);
void prec_stack_pop();
prec_st_element *prec_stack_top();
int prec_stack_is_empty();

void prec_auxstack_init();
int prec_auxstack_push(prec_st_element *);
void prec_auxstack_pop();
prec_st_element prec_auxstack_top();
int prec_auxstack_is_empty();

prec_auxstack_t auxstack;
prec_stack_t stack;

#endif
