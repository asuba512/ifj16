#ifndef PRECEDENCE_H
#define PRECEDENCE_H
#include "token.h"

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

int precedence(tok_que_t);

void prec_stack_init(prec_stack_t *);
int prec_stack_push(prec_stack_t *, token_t);
void prec_stack_pop(prec_stack_t *);
prec_st_element *prec_stack_top(prec_stack_t *);
int prec_stack_is_empty(prec_stack_t *);

void prec_auxstack_init(prec_auxstack_t *);
int prec_auxstack_push(prec_auxstack_t *, prec_st_element *);
void prec_auxstack_pop(prec_auxstack_t *);
prec_st_element prec_auxstack_top(prec_auxstack_t *);
int prec_auxstack_is_empty(prec_auxstack_t *);


#endif
