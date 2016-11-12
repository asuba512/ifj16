#include "token.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_analysis.h"
#include "sym_table.h"
#include "infinite_string.h"
#include "precedence.h"

extern struct temp_data sem_tmp_data;
extern struct fq sem_id_decoded;
extern class_memb_t calling_function;

#define next_token() do{ if(FIRST_PASS){ if((lexerror = get_token(fd, &t)) ) return 2; tok_enqueue(tok_q, t); } else { t = tok_q->head->tok; tok_remove_head(tok_q); }} while(0)
#define is(x) (t.type == x)

/*
TODO:
	- global variable for storing pointers into table of symbols in id()/id1() during SECOND PASS
	- adding literal into TS as variables
	- add inserting in expr queue in other places (maybe function, maybe inline implementation)
	- debugging (a lot)
*/

int c_list(){
	sem_id_decoded.class_id = sem_id_decoded.memb_id = NULL;
	calling_function = NULL;
	next_token();
	if(is(token_k_class)){
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s;
			if (FIRST_PASS)
				sem_new_class(t.attr.s); // first pass adds this class into symbols table
			else if(SECOND_PASS)
				sem_set_active_class(t.attr.s); // second pass marks this class as active scope for searching members
			next_token();
			if(is(token_lbrace)){
				if(!memb_list() && is(token_rbrace)){
					return c_list();
				}
			}
		}
	}
	else if(is(token_eof))
		return 0;
	return 2;
}

int memb_list(){
	active_function = NULL;
	next_token();
	if(is(token_k_static)){
		return c_memb() ||  memb_list() ? 2 : 0;
	}
	else if(is(token_rbrace)){
		return 0;
	}
	return 2;
}

int c_memb(){
	if(is(token_k_static)){
		return c_memb1();
	}
	return 2;
}

int c_memb1(){
	next_token();
	if(is(token_k_void)){
		if(FIRST_PASS)
			sem_tmp_data.dt = t_void;
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s;
			next_token();
			return c_memb_func();
		}
	}
	else if(!type()){
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s;
			return c_memb2();
		}
	}
	return 2;
}

int type(){
	if(is(token_k_int)){
		sem_tmp_data.dt = dt_int;
		return 0;
	} else if (is(token_k_double)) {
		sem_tmp_data.dt = dt_double;
		return 0;
	} else if(is(token_k_string)) {
		sem_tmp_data.dt = dt_String;
		return 0;
	} else if(is(token_k_boolean)){
		sem_tmp_data.dt = dt_boolean;
		return 0;
	}
	return 2;
}

int c_memb_func(){
	if(is(token_lbracket)){
		if (FIRST_PASS)
			sem_add_member_active_class(func); // first pass adds function to symbol table
		else if (SECOND_PASS)
			sem_set_active_fn(t.attr.s); // second pass marks this function as active local scope
		if(!fn_def_plist() && is(token_rbracket)){
			next_token();
			if(is(token_lbrace)){
				if(!fn_body() && is(token_rbrace)){
					return 0;
				}
			}
		}
	}
	return 2;
}

int c_memb2(){
	next_token();
	if(is(token_assign)){
		if (FIRST_PASS)
			sem_add_member_active_class(var);
		do{
			next_token();
		} while(!is(token_semicolon));
		return 0;
	}
	else if(is(token_semicolon)){
		if (FIRST_PASS)
			sem_add_member_active_class(var);
		return 0;
	}
	else if(is(token_lbracket)){
		return c_memb_func();
	}
	return 2;
}

int fn_def_plist(){
	next_token();
	if(is(token_rbracket))
		return 0;
	else if(is(token_k_int) || is(token_k_double) || is(token_k_string) || is(token_k_boolean)){
		if(!par_def()){
			return fn_def_plist1();
		}
	}
	return 2;
}

int fn_def_plist1(){
	next_token();
	if(is(token_comma)){
		next_token();
		if(!par_def()){
			return fn_def_plist1();
		}
	}
	else if(is(token_rbracket)){
		return 0;
	}
	return 2;
}

int par_def(){
	if(!type()){
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s;
			if (FIRST_PASS)
				sem_add_arg_active_fn();		
			return 0;
		}
	}
	return 2;
}

int fn_body(){
	next_token();
	if(is(token_id) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return)){
		if(!stat()){
			return fn_body();
		}
	}
	else if(!type()){
		next_token();
		if(is(token_id)){
			if(SECOND_PASS) {
				sem_new_loc_var(sem_tmp_data.dt, t.attr.s);
				sem_id_decoded.memb_id = t.attr.s;
				sem_id_decoded.class_id = NULL;
				sem_search();
			}
			if(!opt_assign() && is(token_semicolon)){
				return fn_body();
			}
		}
	}
	else if(is(token_rbrace)) {
		active_function = NULL;
		return 0;
	}
	return 2;
}

int opt_assign(){
	next_token();
	if(is(token_assign)){
		op_t tmp_dst;
		if(SECOND_PASS) {
			tmp_dst = (op_t)(sem_id_decoded.ptr);
		}
		int err = assign();
		if(err == 0 && SECOND_PASS) {
			if(!calling_function) {
				if((err = sem_generate_mov(precedence_result, tmp_dst)))
					return err;
			} else {
				if((err = sem_generate_movr(calling_function, tmp_dst)))
					return err;
				calling_function = NULL;
			}
		}
		return err;
	}
	else if(is(token_semicolon)){
		return 0;
	}
	return 2;
}

/* NOT SO HUGE MESS */
int assign(){
	if(FIRST_PASS){ // for test purposes, change to FIRST_PASS
		do{
			next_token();
		} while(!is(token_semicolon));
		return 0;
	}
	if(SECOND_PASS){ // for test purposes, change to SECOND_PASS
		next_token();
		if(!id() && sem_id_decoded.isFun){ // function
			sem_generate_prepare((class_memb_t)sem_id_decoded.ptr);
			if(is(token_lbracket)){
				if(!fn_plist())
					if(is(token_semicolon)){					
						if(!sem_args_ok(calling_function)) {
							return 4;
						}
						sem_generate_call(calling_function);
						return 0;
					}
			}
		}
		else{ // expression
			token_t tmp;
			tok_que_t expr_queue = tok_que_init();
			if(sem_id_decoded.ptr){ // if there was id as first token
				tmp.type = token_id;
				tmp.attr.p = sem_id_decoded.ptr;
				tok_enqueue(expr_queue, tmp);
			}
			else if(t.type > token_string) // unsupported tokens
				return 2;				
			do{	
				if(!is(token_id) && !is(token_int) && !is(token_double) && !is(token_boolean) && !is(token_string)){ // operators
					tok_enqueue(expr_queue, t);
					next_token();
				}
				if(is(token_id)){ // id processing
					if(!id()){ // already next_token, don't need to get another one
						if(sem_id_decoded.ptr == NULL) {
							fprintf(stderr, "ERR: Undefined variable.\n");
							return 4;
						}						
						tmp.type = token_id;
						tmp.attr.p = sem_id_decoded.ptr;
						tok_enqueue(expr_queue, tmp);
					}
					else
						return 2;
				}
				else if(is(token_int) || is(token_double) || is(token_boolean) || is(token_string)){ // literal processing
					tmp.type = token_id;
					tmp.attr.p = add_literal(t); // insert as variable into TS
					tok_enqueue(expr_queue, tmp);
					next_token();	
				}
			} while(t.type <= token_string);
			tmp.type = token_eof; // add eof to the end of queue
			tok_enqueue(expr_queue, tmp);
			return precedence(expr_queue, &precedence_result);
		}
	}
	return 2;
}

int fn_plist(){
	next_token();
	if(SECOND_PASS)
		sem_rst_argcount();
	if(is(token_rbracket)){
		next_token();
		return 0;
	}
	else if(!val_id()){
		return fn_plist1();
	}
	return 2;
}

int fn_plist1(){
	if(is(token_rbracket)){
		next_token();
		return 0;
	}
	else if(is(token_comma)){
		next_token();
		if(!val_id())
			return fn_plist1();
	}
	return 2;
}

int val_id(){
	if(!id()){
		if(SECOND_PASS) {
			if(calling_function) {
				sem_generate_push(calling_function, (op_t)sem_id_decoded.ptr);
			}
		}
		return 0;
	}
	if(is(token_int) || is(token_double) || is(token_string) || is(token_boolean)){
		if(SECOND_PASS) {
			if(calling_function) {
				op_t literal = (op_t)add_literal(t);
				//printf("Literal: %d (%p)\n", ((literal_t)literal)->i_val, (void*)literal);
				sem_generate_push(calling_function, literal);
			}
		}
		next_token();
		return 0;
	}
	return 2;
}

int id(){
	if(is(token_id)){
		return id1();
	}
	sem_id_decoded.ptr = NULL;
	return 2;
}

int id1(){
	if(SECOND_PASS)
		sem_id_decoded.class_id = t.attr.s;
	next_token();
	if(is(token_dot)){
		next_token();
		if(is(token_id)){
			if(SECOND_PASS) {
				sem_id_decoded.memb_id = t.attr.s;
				sem_search();
			}
			next_token();
			return 0;	
		}
	}
	else if(is(token_lbracket) || is(token_rbracket) || is(token_assign) || is(token_comma) || is(token_addition) || is(token_substraction) || is(token_multiplication) || is(token_division) || is(token_less) || is(token_more) || is(token_lesseq) || is(token_moreeq) || is(token_equal) || is(token_nequal) || is(token_and) || is(token_or) || is(token_not) || is(token_semicolon)){
		if(SECOND_PASS) {
			sem_id_decoded.memb_id = sem_id_decoded.class_id;
			sem_id_decoded.class_id = NULL;
			sem_search();
		}		
		return 0;
	}
	if(SECOND_PASS)
		sem_id_decoded.ptr = NULL;
	return 2;
}

int stat(){
	int lb = 0, rb = 0;
	if(is(token_id)){
		if(!id()){
			if(!as_ca() && is(token_semicolon))
				return 0;
		}
	}
	else if(is(token_lbrace)){
		return stat_com();
	}
	else if(is(token_k_if)){
		next_token();
		if(is(token_lbracket)){
			tok_que_t expr_queue = tok_que_init();
			token_t tmp;
			if(SECOND_PASS){
				do{
					if(!is(token_id) && !is(token_int) && !is(token_double) && !is(token_boolean) && !is(token_string)){ // operators
						if(is(token_lbracket)) lb++; // pairing brackets
						if(is(token_rbracket)) rb++;
						tok_enqueue(expr_queue, t);
						next_token();
					}
					if(is(token_id)){ // id processing
						if(!id()){ // already next_token, don't need to get another one
							tmp.type = token_id;
							tmp.attr.p = sem_id_decoded.ptr;
							tok_enqueue(expr_queue, tmp);
							}
						else
							return 2;
					}
					else if(is(token_int) || is(token_double) || is(token_boolean) || is(token_string)){ // literal processing
						tmp.type = token_id;
						tmp.attr.p = add_literal(t); // insert as variable into TS
						tok_enqueue(expr_queue, tmp);
						next_token();	
					}
				} while((lb != rb || is(token_rbracket)) && t.type <= token_string);
				tmp.type = token_eof; // add eof to the end of queue
				tok_enqueue(expr_queue, tmp);
				if(precedence(expr_queue, &precedence_result)) // in case of precedence error, will have to use error var to distinguish this error
					return 2;
			}
			else{
				do{ // in first pass skip the whole expression
					if(is(token_lbracket)) lb++;
					if(is(token_rbracket)) rb++;
					next_token();
				} while((lb != rb || is(token_rbracket)) && t.type <= token_string);
			}
			if(!stat_com()){
				next_token();
				if(is(token_k_else)){
					next_token();
					if(!stat_com()){
						return 0;
					}
				}		
			}
		}
	}
	else if(is(token_k_while)){
		next_token();
		if(is(token_lbracket)){
			lb++;
			do{
				next_token();
				if(is(token_lbracket)) lb++;
				if(is(token_rbracket)) rb++;
			} while(lb != rb || !is(token_rbracket));
			lb = rb = 0;
			next_token();
			if(!stat_com()){
				return 0;
			}
		}
	}
	else if(is(token_k_return)){
		if(!ret_val() && is(token_semicolon)){
			return 0;
		}
	}
	return 2;
}

int stat_com(){
	if(is(token_lbrace)){
		if(!stat_list() && is(token_rbrace)){
			return 0;
		}
	}
	return 2;
}

int stat_list(){
	next_token();
	if(is(token_id) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return)){
		if(!stat()){
			return stat_list();
		}
	}
	else if(is(token_rbrace)){
		return 0;
	}
	return 2;
}

int as_ca(){
	if(is(token_lbracket)){
		return fn_plist();
	}
	else if(is(token_assign)) {
		op_t tmp_dst;
		if(SECOND_PASS) {
			tmp_dst = (op_t)(sem_id_decoded.ptr);
		}
		int err = assign();
		if(err == 0 && SECOND_PASS) {
			if(!calling_function) {
				if((err = sem_generate_mov(precedence_result, tmp_dst)))
					return err;
			} else {
				if((err = sem_generate_movr(calling_function, tmp_dst)))
					return err;
				calling_function = NULL;
			}
		}
		return err;
	}
	return 2;
}

int ret_val(){
	do{
		next_token();
	} while(!is(token_semicolon));
	return 0;
}
