#include "token.h"

typedef struct prec_stack_element{
	token_t data;
	struct prec_stack_element *next;
} *prec_st_element;

typedef struct prec_stack{
	prec_st_element top;
} prec_stack_t;

void prec_init(prec_stack_t *);
int prec_push(prec_stack_t *, token_t);
void prec_pop(prec_stack_t *);
token_t prec_top(prec_stack_t *);
int prec_is_empty(prec_stack_t *);
