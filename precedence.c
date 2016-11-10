#include "precedence.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

/*
TODO & explanations:
	- there should be some tmp_var, which will be pointer into table of symbols,
		the purpose is storing nonterminal attribute, which would be already lost, when you generate instruction
	- there are only two stacks with different methods, I made them global and methods are called without name of stack
	- debugging
*/

int precedence(tok_que_t queue){
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
	
	prec_stack_push(eof); 

	qtoken = tok_remove_head(queue);
	stoken = (*prec_stack_top())->data;
	do{
		switch(prec_table[stoken.type][qtoken.type]){
			case '=':
				prec_stack_push(qtoken);
				qtoken = tok_remove_head(queue);
				break;
			case '<': // insert <
				prec_auxstack_push(prec_stack_top());
				prec_stack_push(qtoken);
				qtoken = tok_remove_head(queue);
				break;
			case '>': // reduce
				if(!prec_auxstack_is_empty()){
					tmp = stack.top;
					if(tmp && tmp->data.type == token_id && prec_auxstack_top() == tmp){ // E -> id
						nonterminal.attr.p = tmp->data.attr.p; // kinda semantic action, it's probably what you wanna do
						prec_stack_pop();
						prec_stack_push(nonterminal);
						prec_auxstack_pop(); 
					}
					else if(tmp && tmp->data.type == token_rbracket){ // E -> (E)
						printf("(");
						prec_stack_pop();
						tmp = stack.top;
						if(tmp && tmp->data.type == token_nonterminal){
							printf("E");// *((int*)(tmp->data.attr.p))); // needs to be stored in tmp_var (doesn't exist yet)
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_lbracket){
								printf(")\n");
								prec_stack_pop();
								//nonterminal.attr.p =tmp_var // no instruction needed, just transfer nonterminal
								prec_stack_push(nonterminal);
								prec_auxstack_pop();
							}
						}
					}
					else if(tmp && tmp->data.type == token_nonterminal){ // E -> E ? E
						printf("E"); // *((int*)(tmp->data.attr.p))); // needs to be stored in tmp_var (doesn't exist yet)
						prec_stack_pop(); // move to next token on stack
						tmp = stack.top;
						if(tmp && tmp->data.type == token_addition){ // E -> E + E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("+E\n");// *((int*)(tmp->data.attr.p))); // generate (+, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_substraction){ // E -> E - E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("-E\n");// *((int*)(tmp->data.attr.p))); // generate (-, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_multiplication){ // E -> E * E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("*E\n");// *((int*)(tmp->data.attr.p))); // generate (*, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_division){ // E -> E / E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("/E\n");// *((int*)(tmp->data.attr.p))); // generate (/, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_less){ // E -> E < E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("<E\n");// *((int*)(tmp->data.attr.p))); // generate (<, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_more){ // E -> E > E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf(">E\n");// *((int*)(tmp->data.attr.p))); // generate (>, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_lesseq){ // E -> E <= E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("<=E\n");// *((int*)(tmp->data.attr.p))); // generate (<=, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_moreeq){ // E -> E >= E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf(">=E\n");// *((int*)(tmp->data.attr.p))); // generate (>=, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_equal){ // E -> E == E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("==E\n");// *((int*)(tmp->data.attr.p))); // generate (==, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_nequal){ // E -> E != E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("!=E\n");// *((int*)(tmp->data.attr.p))); // generate (!=, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_and){ // E -> E && E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("&&E\n");// *((int*)(tmp->data.attr.p))); // generate (&&, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_or){ // E -> E || E
							prec_stack_pop();
							tmp = stack.top;
							if(tmp && tmp->data.type == token_nonterminal && prec_auxstack_top()->prev == tmp){
								printf("||E\n");// *((int*)(tmp->data.attr.p))); // generate (||, tmp_var, tmp->data.attr.p), result into new variable
								prec_stack_pop();
								//nonterminal.attr.p = result variable from instruction above
								prec_stack_push(nonterminal);
								prec_auxstack_pop(); 
							}
						}
						else if(tmp && tmp->data.type == token_not && prec_auxstack_top() == tmp){ // E -> !E
							printf("!\n"); // generate (!, tmp_var), result into new variable
							prec_stack_pop();
							//nonterminal.attr.p = result variable from instruction above
							prec_stack_push(nonterminal);
							prec_auxstack_pop(); 
						}
						else
							return 222;
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
		stoken = (*prec_stack_top())->data;
	} while(qtoken.type != token_eof || stoken.type != token_eof);
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
	if((tmp = malloc(sizeof(struct prec_stack_element)))){
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
	if((tmp = malloc(sizeof(struct prec_auxstack_element)))){
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
		free(stack.top);
		stack.top = tmp;
		stack.top->prev = NULL;
	}
}

void prec_auxstack_pop(){
	if(auxstack.top){
		prec_auxst_element tmp = auxstack.top->next;
		free(auxstack.top);
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
