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
	static token_t stop = {.type = token_stop}; // auxiliary token representing stop
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
				prec_stack_insert(prec_stack_top(), stop);
				if(prec_stack_push(qtoken)) return 99;
				qtoken = tok_remove_head(queue);
				break;
			case '>': // reduce
				/* many parts of this case are similar to each other, only in different order, it isn't necessary to comment everything' */
				if(count_of_stops){ // if there is something to reduce
					tmp = stack.top; // get the actual top of stack (top() returns top terminal !!!)
					if(tmp && tmp->data.type == token_id){ // E -> id
						nonterminal.attr.p = tmp->data.attr.p;
						prec_stack_pop(); // remove already processed token
						tmp = stack.top;
						if(tmp && tmp->data.type == token_stop){
							prec_stack_pop(); // finish the reduction
							count_of_stops--; // stop was removed
							if(prec_stack_push(nonterminal)) return 99;
						}
						else
							return 2;
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
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99;
								}
								else return 2;
							}
						}
					}
					/* E -> E ? E */
					else if(tmp && tmp->data.type == token_nonterminal){ // E
						op_t op1, op2;
						op2 = (op_t)(tmp->data.attr.p);
						prec_stack_pop(); // move to next token on stack
						tmp = stack.top;
						/* list of possible binary operators follow, the body of if/elseif is always the same */ 
						if(tmp && tmp->data.type == token_addition){ // E + 
							prec_stack_pop(); // move to next token on stack
							tmp = stack.top; // get the actual top of stack (prec_stack_top() returns top terminal)
							/* at this point, on the top of stack should be nonterminal */
							if(tmp && tmp->data.type == token_nonterminal){ // E + E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop(); // remove operand from stack
								try(sem_generate_arithm(add, op1, op2, (op_t *)&(nonterminal.attr.p))); // generate instruction
								tmp = stack.top; // next token on stack should be token_stop
								if(tmp && tmp->data.type == token_stop){ 
									prec_stack_pop(); // finish the reduction
									count_of_stops--; // stop was removed
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_substraction){ // E -
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E - E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(sub, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_multiplication){ // E *
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E * E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(imul, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99;
								}
								else return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_division){ // E /
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E / E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(idiv, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_less){ // E <
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E < E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(less, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_more){ // E >
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E > E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(gre, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_lesseq){ // E <=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E <= E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(leq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_moreeq){ // E >=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E >= E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(geq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_equal){ // E ==
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E == E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(eql, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_nequal){ // E !=
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E != E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(neq, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_and){ // E &&
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E && E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(and, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_or){ // E ||
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal){ // E || E
								op1 = (op_t)(tmp->data.attr.p);
								prec_stack_pop();
								try(sem_generate_arithm(or, op1, op2, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
							}
							else
								return 2;
						}
						else if(tmp && tmp->data.type == token_not){ // E -> !E
							prec_stack_pop();
							try(sem_generate_arithm(not, op2, NULL, (op_t *)&(nonterminal.attr.p)));
								tmp = stack.top;
								if(tmp && tmp->data.type == token_stop){
									prec_stack_pop();
									count_of_stops--;
									if(prec_stack_push(nonterminal)) return 99; // push the result
								}
								else
									return 2;
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
	count_of_stops = 0;
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


/*
	Inserts token into stack in front of passed element
*/
int prec_stack_insert(prec_st_element *e,token_t t){
	prec_st_element tmp;
	if((tmp = gc_malloc(sizeof(struct prec_stack_element)))){
		tmp->data = t;
		tmp->next = *e;
		tmp->prev = (*e)->prev;
		if((*e)->prev)
			(*e)->prev->next = tmp;
		else
			stack.top = tmp;
		(*e)->prev = tmp;
		count_of_stops++;
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

/*
	Returns top terminal on stack
*/
prec_st_element *prec_stack_top(){
	prec_st_element *tmp = &stack.top;
	while((*tmp)->data.type == token_nonterminal)
		tmp = &(*tmp)->next;
	return tmp;
}

/*
	Returns 1 if stack is empty, otherwise 0
*/
int prec_stack_is_empty(){
	if(stack.top)
		return 0;
	return 1;
}
