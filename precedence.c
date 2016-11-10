#include "precedence.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

/*
TODO:
	- whole precedence() algorithm
	- stack for storing positions of < symbols in main stack
	- debugging
*/

int precedence(tok_que_t queue){
	prec_stack_t t_stack;
	prec_auxstack_t a_stack;
	prec_stack_init(&t_stack);
	prec_auxstack_init(&a_stack);

	static char prec_table[17][17] = {
		{'>','>','<','<','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','<','<','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','X','X','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','X','X','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','=','<','<','<','<','X'},
		{'>','>','>','>','>','>','>','>','>','>','X','>','>','>','>','X','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','>','<','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','X','>','>','>','>','X','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','X','<','<','<','<','X'},
	};
	static token_t eof = {.type = token_eof};
	static token_t nonterminal = {.type = token_nonterminal};
	token_t qtoken, stoken;
	prec_st_element tmp;
	
	prec_stack_push(&t_stack, eof); 

	qtoken = tok_remove_head(queue);
	stoken = (*prec_stack_top(&t_stack))->data;
	do{
		printf("top terminal: %d\n", stoken.type);
		switch(prec_table[stoken.type][qtoken.type]){
			case '=':
				prec_stack_push(&t_stack, eof);
				qtoken = tok_remove_head(queue);
				break;
			case '<':
				prec_auxstack_push(&a_stack, prec_stack_top(&t_stack));
				prec_stack_push(&t_stack, qtoken);
				qtoken = tok_remove_head(queue);
				break;
			case '>':
				if(!prec_auxstack_is_empty(&a_stack)){
					tmp = t_stack.top;
					if(tmp && tmp->data.type == token_id && prec_auxstack_top(&a_stack) == tmp){
						nonterminal.attr.p = tmp->data.attr.p; // kinda semantic action
						prec_stack_pop(&t_stack);
						prec_stack_push(&t_stack, nonterminal);
						prec_auxstack_pop(&a_stack); 
					}
					else if(tmp && tmp->data.type == token_nonterminal){
						printf("%d", *((int*)(tmp->data.attr.p)));
						prec_stack_pop(&t_stack);
						tmp = t_stack.top;
						if(tmp && tmp->data.type == token_addition){
							prec_stack_pop(&t_stack);
							tmp = t_stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top(&a_stack)->prev == tmp){
								printf("+%d\n", *((int*)(tmp->data.attr.p)));
								prec_stack_pop(&t_stack);
								prec_stack_push(&t_stack, nonterminal);
								prec_auxstack_pop(&a_stack); 
							}
						}
						else if(tmp && tmp->data.type == token_substraction){
							prec_stack_pop(&t_stack);
							tmp = t_stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top(&a_stack)->prev == tmp){
								printf("-%d\n", *((int*)(tmp->data.attr.p)));
								prec_stack_pop(&t_stack);
								prec_stack_push(&t_stack, nonterminal);
								prec_auxstack_pop(&a_stack); 

							}
						}
						else if(tmp && tmp->data.type == token_multiplication){
							prec_stack_pop(&t_stack);
							tmp = t_stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top(&a_stack)->prev == tmp){
								printf("*%d\n", *((int*)(tmp->data.attr.p)));
								prec_stack_pop(&t_stack);
								prec_stack_push(&t_stack, nonterminal);
								prec_auxstack_pop(&a_stack); 

							}
						}
					}
					else
						return 111;
				}
				else
					return 351;
				break;
			default:
				return 32453;
				break;
		}
		stoken = (*prec_stack_top(&t_stack))->data;
	} while(qtoken.type != token_eof || stoken.type != token_eof);
	return 0;
}
/*
	Initializes stack to empty stack
*/
void prec_stack_init(prec_stack_t *s){
	s->top = NULL;
}
void prec_auxstack_init(prec_auxstack_t *s){
	s->top = NULL;
}

/*
	Pushes new element into stack.
	Return value: 0 means succes, 1 means error.
*/
int prec_stack_push(prec_stack_t *s, token_t t){
	prec_st_element tmp;
	if((tmp = malloc(sizeof(struct prec_stack_element)))){
		tmp->data = t;
		tmp->next = s->top;
		if(s->top != NULL)
			tmp->next->prev = tmp;
		tmp->prev = NULL;
		s->top = tmp;
		return 0;
	}
	return 1;
}

int prec_auxstack_push(prec_auxstack_t *s, prec_st_element *e){
	prec_auxst_element tmp;
	if((tmp = malloc(sizeof(struct prec_auxstack_element)))){
		tmp->ptr = e;
		tmp->next = s->top;
		s->top = tmp;
		return 0;
	}
	return 1;
}

/*
	Pops an element from stack.
*/
void prec_stack_pop(prec_stack_t *s){
	if(s->top){
		prec_st_element tmp = s->top->next;
		free(s->top);
		s->top = tmp;
		s->top->prev = NULL;
	}
}

void prec_auxstack_pop(prec_auxstack_t *s){
	if(s->top){
		prec_auxst_element tmp = s->top->next;
		free(s->top);
		s->top = tmp;
	}
}

/*
	Returns value on top of stack
*/
prec_st_element *prec_stack_top(prec_stack_t *s){
	prec_st_element *tmp = &s->top;
	while((*tmp)->data.type == token_nonterminal)
		tmp = &(*tmp)->next;
	return tmp;
}

prec_st_element prec_auxstack_top(prec_auxstack_t *s){
	return (*s->top->ptr);
}

/*
	Returns 1 if stack is empty, otherwise 0
*/
int prec_stack_is_empty(prec_stack_t *s){
	if(s->top)
		return 0;
	return 1;
}

int prec_auxstack_is_empty(prec_auxstack_t *s){
	if(s->top)
		return 0;
	return 1;
}
