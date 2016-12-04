#include "precedence.h"
#include "token.h"
#include "sym_table.h"
#include "semantic_analysis.h"
#include <stdlib.h>
#include <stdio.h>
#include "gc.h"

#define try(x) if((err = x)) return err

extern int error_number;

int precedence(tok_que_t queue, op_t *result){
	int err;
	/* initialization of prec_table (static) and stacks */
	prec_stack_init();
	prec_auxstack_init();
	static char prec_table[17][17] = {
		{'>','>','<','<','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','<','<','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','X','X','X','X','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','>','>','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','=','<','<','<','<','X'},
		{'>','>','>','>','>','>','>','>','>','>','X','>','>','>','>','X','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','>','>','>','<','<','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','>','<','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','<','>','>','>','<','<','>'},
		{'>','>','>','>','>','>','>','>','>','>','X','>','>','>','>','X','>'},
		{'<','<','<','<','<','<','<','<','<','<','<','X','<','<','<','<','X'},
	};

	static token_t eof = {.type = token_eof}; // auxiliary token representing EOF
	static token_t nonterminal = {.type = token_nonterminal}; // auxiliary token representing non-terminal E
	token_t qtoken, stoken;
	prec_st_element tmp;
	
	if(prec_stack_push(eof)) return 99; // first element of precedence stack must be EOF

	qtoken = tok_remove_head(queue);
	stoken = (*prec_stack_top())->data;
	do{
		switch(prec_table[stoken.type][qtoken.type]){
			case '=':
				if(prec_stack_push(qtoken)) return 99;
				qtoken = tok_remove_head(queue);
				break;
			case '<': // insert <
				if(prec_auxstack_push(prec_stack_top())) return 99;
				if(prec_stack_push(qtoken)) return 99;
				qtoken = tok_remove_head(queue);
				break;
			case '>': // reduce
				if(!prec_auxstack_is_empty()){ // if there is something to reduce
					tmp = stack.top;
					if(tmp && tmp->data.type == token_id && prec_auxstack_top() == tmp){ // E -> id
						nonterminal.attr.p = tmp->data.attr.p; // kinda semantic action, it's probably what you wanna do
						prec_stack_pop();
						if(prec_stack_push(nonterminal)) return 99;
						prec_auxstack_pop(); 
					}
					/* E -> (E) */
					else if(tmp && tmp->data.type == token_rbracket){ // (
						prec_stack_pop();
						tmp = stack.top;
						if(tmp && tmp->data.type == token_nonterminal){ // (E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_lbracket){ // (E)
								prec_stack_pop();
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop();
							}
						}
					}
					/* E -> E ? E */
					else if(tmp && tmp->data.type == token_nonterminal){ // E
						op_t op1, op2;
						op2 = (op_t)(tmp->data.attr.p);
						prec_stack_pop(); // move to next token on stack
						tmp = stack.top;
						if(tmp && tmp->data.type == token_addition){ // E + 
							prec_stack_pop(); // move to next token on stack
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E + E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(add, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_substraction){ // E -
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E - E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(sub, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_multiplication){ // E *
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E * E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(imul, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_division){ // E /
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E / E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(idiv, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_less){ // E <
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E < E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(less, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_more){ // E >
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E > E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(gre, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_lesseq){ // E <=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E <= E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(leq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_moreeq){ // E >=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E >= E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(geq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_equal){ // E ==
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E == E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(eql, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_nequal){ // E !=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E != E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(neq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_and){ // E &&
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E && E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(and, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_or){ // E ||
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){ // E || E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(or, op1, op2, (op_t *)&(nonterminal.attr.p)));
								if(prec_stack_push(nonterminal)) return 99;
								prec_auxstack_pop(); 
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_not && prec_auxstack_top() == tmp){ // E -> !E
							prec_stack_pop();
							try(sem_generate_arithm(not, op2, NULL, (op_t *)&(nonterminal.attr.p)));
							if(prec_stack_push(nonterminal)) return 99;
							prec_auxstack_pop(); 
						}
						else
							return 2;
					}
					else
						return 2;
				}
				else
					return 2;
				break;
			default:
				return 2;
				break;
		}
		stoken = (*prec_stack_top())->data;
	} while(qtoken.type != token_eof || stoken.type != token_eof); // both queue and stack must contain only EOF
	*result = nonterminal.attr.p; // returning the result from precedence
	return 0;
}
/*
	Initializes stack to empty stack
*/
void prec_stack_init(){
	stack.top = NULL;
}
void prec_auxstack_init(){
	auxstack.top = NULL;
}

/*
	Pushes new element into stack.
	Return value: 0 means succes, 1 means error.
*/
int prec_stack_push(token_t t){
	prec_st_element tmp;
	if((tmp = gc_malloc(sizeof(struct prec_stack_element)))){
		tmp->data = t;
		tmp->next = stack.top;
		if(stack.top != NULL)
			tmp->next->prev = tmp;
		tmp->prev = NULL;
		stack.top = tmp;
		return 0;
	}
	return 1;
}

int prec_auxstack_push(prec_st_element *e){
	prec_auxst_element tmp;
	if((tmp = gc_malloc(sizeof(struct prec_auxstack_element)))){
		tmp->ptr = e;
		tmp->next = auxstack.top;
		auxstack.top = tmp;
		return 0;
	}
	return 1;
}

/*
	Pops an element from stack.
*/
void prec_stack_pop(){
	if(stack.top){
		prec_st_element tmp = stack.top->next;
		//free(stack.top);
		stack.top = tmp;
		stack.top->prev = NULL;
	}
}

void prec_auxstack_pop(){
	if(auxstack.top){
		prec_auxst_element tmp = auxstack.top->next;
		//free(auxstack.top);
		auxstack.top = tmp;
	}
}

/*
	Returns value on top of stack
*/
prec_st_element *prec_stack_top(){
	prec_st_element *tmp = &stack.top;
	while((*tmp)->data.type == token_nonterminal)
		tmp = &(*tmp)->next;
	return tmp;
}

prec_st_element prec_auxstack_top(){
	return (*auxstack.top->ptr);
}

/*
	Returns 1 if stack is empty, otherwise 0
*/
int prec_stack_is_empty(){
	if(stack.top)
		return 0;
	return 1;
}

int prec_auxstack_is_empty(){
	if(auxstack.top)
		return 0;
	return 1;
}
