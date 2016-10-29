#include "precedence.h"
#include "token.h"
#include <stdlib.h>

/*
TODO:
	- whole precedence() algorithm
	- stack for storing positions of < symbols in main stack
	- debugging
*/

int precedence(tok_que_t queue){
	prec_stack_t stack;
	prec_init(&stack);
	tok_que_destroy(queue);
	return 0;
}

/*
	Initializes queue to empty queue
*/
void prec_init(prec_stat_t *s){
	s->top = NULL;
}

/*
	Pushes new element into stack.
	Return value: 0 means succes, 1 means error.
*/
int s_push(prec_stack_t *s, token_t t){
	prec_st_element tmp;
	if((tmp = malloc(sizeof(struct element)))){
		tmp->data = t;
		tmp->next = s->top;
		s->top = tmp;
		return 0;
	}
	return 1;
}

/*
	Pops an element from stack.
*/
void s_pop(prec_stack_t *s){
	if(s->top){
		s_element tmp = s->top->next;
		free(s->top);
		s->top = tmp;
	}
}

/*
	Returns value on top of stack
*/
token_t s_top(prec_stack_t *s){
	return s->top->data;
}

/*
	Returns 1 if stack is empty, otherwise 0
*/
int prec_is_empty(prec_stack_t *s){
	if(s->top)
		return 0;
	return 1;
}
