#include "token.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_analysis.h"
#include "sym_table.h"
#include "infinite_string.h"
#include "precedence.h"
#include "ifj16_class.h"
#include <string.h>

// OPTIMALIZATION FLAG = marks places that can be optimized

extern struct temp_data sem_tmp_data;
extern struct fq sem_id_decoded;
extern class_memb_t calling_function;
extern class_memb_t print_fn;
extern op_t precedence_result;
extern bool outside_func;

#define next_token() do{ if(FIRST_PASS){ if((errno = get_token(fd, &t)) ) return 2; tok_enqueue(tok_q, t); } else { t = tok_q->head->tok; tok_remove_head(tok_q); }} while(0)
#define is(x) (t.type == x)

int c_list(){
	outside_func = true;
	sem_id_decoded.class_id = sem_id_decoded.memb_id = NULL;
	sem_id_decoded.ptr = NULL;
	calling_function = NULL;
	next_token();
	if(is(token_k_class)){
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s;
			if (FIRST_PASS) {
				errno = sem_new_class(t.attr.s); // first pass adds this class into symbols table
				if (errno) return errno;
			}
			else if(SECOND_PASS) // OPTIMALIZATION FLAG = redundant search
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
		if (FIRST_PASS) {
			errno = sem_add_member_active_class(func); // first pass adds function to symbol table
			if (errno) return errno;
		}
		else if (SECOND_PASS)
			sem_set_active_fn(t.attr.s); // second pass marks this function as active local scope
		if(!fn_def_plist() && is(token_rbracket)){
			next_token();
			if(is(token_lbrace)){
				outside_func = false;
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
		if (FIRST_PASS) {
			if((errno = sem_add_member_active_class(var))) return errno;
			class_memb_t tmp_dst = st_getmemb(active_class, sem_tmp_data.id);
			next_token();
			token_t tmp;
			tok_que_t expr_queue = tok_que_init();
			if(sem_id_decoded.ptr){ // if there was id as first token
				tmp.type = token_id;
				tmp.attr.p = sem_id_decoded.ptr;
				tok_enqueue(expr_queue, tmp);
				next_token(); // if there was an id(), you should take another token
			}
			else if(t.type > token_string){ // unsupported tokens
				return 2;				
			}
			if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
				return 2;
			errno = precedence(expr_queue, &precedence_result);
			if(!errno) {
				if((errno = sem_generate_mov(precedence_result, (op_t)tmp_dst)))
					return errno;
			}
			return errno;
		} else {
			do{
				next_token();
			} while(!is(token_semicolon));
		}
		return 0;
	}
	else if(is(token_semicolon)){
		if (FIRST_PASS)
			if ((errno = sem_add_member_active_class(var))) return errno;
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
			if (FIRST_PASS) {
				errno = sem_add_arg_active_fn();
				if(errno) return errno;
			}
			return 0;
		}
	}
	return 2;
}

int fn_body(){
	next_token();
	if(is(token_id) || is(token_fqid) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return)){
		if(!stat()){
			return fn_body();
		}
	}
	else if(!type()){
		next_token();
		if(is(token_id)){
			if(SECOND_PASS) {
				errno = sem_new_loc_var(sem_tmp_data.dt, t.attr.s);
				if(errno) return errno;
				// OPTIMALIZATION FLAG - redundant search
				sem_id_decoded.memb_id = t.attr.s;
				sem_id_decoded.class_id = NULL;
				sem_search(); // never causes error
			}
			if(!opt_assign() && is(token_semicolon)){
				return fn_body();
			}
		}
	}
	else if(is(token_rbrace)) {
		if(SECOND_PASS) {
			if(active_function->dtype == t_void) {
				if ((errno = sem_generate_ret(NULL))) return errno; // append ret instruction at the end of void function
			}
			else {
				if ((errno = sem_generate_halt())) return errno; // append halt instruction at the end of non-void function. proper funcion should not reach this instruction
			}
				
		}
		outside_func = true;
		active_function = NULL;
		return 0;
	}
	return 2;
}

int opt_assign(){
	next_token();
	if(is(token_assign)){
		op_t tmp_dst = NULL;
		if(SECOND_PASS) {
			tmp_dst = (op_t)(sem_id_decoded.ptr);
		}
		int err = assign();
		if(err == 0 && SECOND_PASS) {
			if(!calling_function) {
				if((errno = sem_generate_mov(precedence_result, tmp_dst)))
					return errno;
			} else {
				if((errno = sem_generate_movr(calling_function, tmp_dst)))
					return errno;
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
		} while(t.type <= token_string || is(token_comma));
		return 0;
	}
	if(SECOND_PASS){ // for test purposes, change to SECOND_PASS
		next_token();
		if(!id() && sem_id_decoded.isFun){ // function
			if(((class_memb_t)sem_id_decoded.ptr)->dtype == t_void) {
				fprintf(stderr, "ERR: Cannot assign return value of void-function.\n");
				return errno = 8;
			}
			if((errno = sem_generate_prepare((class_memb_t)sem_id_decoded.ptr))) return errno;
			next_token();
			if(is(token_lbracket)){
				if(!fn_plist())
					if(is(token_semicolon)){					
						if(!sem_args_ok(calling_function)) {
							return 4;
						}
						if((errno = sem_generate_call(calling_function))) return errno;
						return 0;
					}
			}
		}
		else if(errno) {
			return errno;
		}
		else{ // expression
			token_t tmp;
			tok_que_t expr_queue = tok_que_init();
			if(sem_id_decoded.ptr){ // if there was id as first token
				tmp.type = token_id;
				tmp.attr.p = sem_id_decoded.ptr;
				tok_enqueue(expr_queue, tmp);
				next_token(); // if there was an id(), you should take another token
			}
			else if(t.type > token_string){ // unsupported tokens
				return 2;				
			}
			if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
				return 2;
			return errno = precedence(expr_queue, &precedence_result);
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
	else if(!val_id())
		return fn_plist1();
	return 2;
}

int fn_plist1(){
	next_token();
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
				if ((errno = sem_generate_push(calling_function, (op_t)sem_id_decoded.ptr))) return errno;
			}
		}
		return 0;
	}
	if(is(token_int) || is(token_double) || is(token_string) || is(token_boolean)){
		if(SECOND_PASS) {
			if(calling_function) {
				op_t literal = (op_t)add_global_helper_var(t, true);
				//printf("Literal: %d (%p)\n", ((literal_t)literal)->i_val, (void*)literal);
				if ((errno = sem_generate_push(calling_function, literal))) return errno;
			}
		}
		return 0;
	}
	return 2;
}

int id(){
	if(is(token_id)){
		if(SECOND_PASS || (FIRST_PASS && outside_func)) {
			sem_id_decoded.memb_id = t.attr.s;
			sem_id_decoded.class_id = NULL;
			sem_search();
			if(!sem_id_decoded.ptr) {
				fprintf(stderr, "ERR: Unknown identifier %s\n", sem_id_decoded.memb_id->data);
				return errno = 3;
			}
		}
		return 0;
	}
	else if(is(token_fqid)){
		if(SECOND_PASS || (FIRST_PASS && outside_func)) {
			char *ptr = strchr(t.attr.s->data, '.');
			*ptr = 0;
			sem_id_decoded.class_id = str_init(t.attr.s->data);
			sem_id_decoded.memb_id = str_init(ptr + 1);
			str_destroy(t.attr.s);
			sem_search();
			if(!sem_id_decoded.ptr) {
				fprintf(stderr, "ERR: Unknown identifier %s.%s\n", sem_id_decoded.class_id->data, sem_id_decoded.memb_id->data);
				return errno = 3;
			}
		}
		return 0;
	}
	if(SECOND_PASS || (FIRST_PASS && outside_func))
		sem_id_decoded.ptr = NULL;
	return 2;
}

int stat(){
	int lb = 0, rb = 0;
	if(is(token_id) || is(token_fqid)){
		if(!id()){
			next_token();
			if(!as_ca() && is(token_semicolon))
				return 0;
		}
	}
	else if(is(token_lbrace)){
		return stat_com();
	}
	else if(is(token_k_if)){
		instr_t if_jmp_instr = NULL;
		instr_t pre_else_jmp = NULL;
		next_token();
		if(is(token_lbracket)){
			tok_que_t expr_queue = tok_que_init(); // init new queue
			if(SECOND_PASS){
				if(_cond_fill_que(expr_queue, true)) // fill queue, counting brackets is on
					return 2;
				if((errno = precedence(expr_queue, &precedence_result)))
					return 2;
				if((errno = sem_generate_jmpifn(precedence_result))) return errno;
				if_jmp_instr = active_function->instr_list_end; // we'll add jump target later
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
				if(is(token_k_else)) {
					if(SECOND_PASS) {
						if ((errno = sem_generate_jmp(NULL))) return errno;
						pre_else_jmp = active_function->instr_list_end; // another jump we will correct later
						if ((errno = sem_generate_label())) return errno;
						sem_set_jmp_dst(if_jmp_instr, (op_t)(active_function->instr_list_end)); //set label we just created as a jump target
					}					
					next_token();
					if(!stat_com()){
						if(SECOND_PASS) {
							if ((errno = sem_generate_label())) return errno;
							sem_set_jmp_dst(pre_else_jmp, (op_t)(active_function->instr_list_end));
						}
						
						return 0;
					}
				}		
			}
		}
	}
	else if(is(token_k_while)){
		instr_t while_label = NULL, while_jmp = NULL;
		if(SECOND_PASS) {
			if ((errno = sem_generate_label())) return errno;
			while_label = active_function->instr_list_end;
		}
		next_token();
		if(is(token_lbracket)){
			tok_que_t expr_queue = tok_que_init(); // init new queue
			if(SECOND_PASS){
				if(_cond_fill_que(expr_queue, true)) // fill queue, counting brackets is on
					return 2;
				if((errno = precedence(expr_queue, &precedence_result)))
					return errno;
				if ((errno = sem_generate_jmpifn(precedence_result))) return errno;
				while_jmp = active_function->instr_list_end;
			}
			else{
				do{ // in first pass skip the whole expression
					if(is(token_lbracket)) lb++;
					if(is(token_rbracket)) rb++;
					next_token();
				} while((lb != rb || is(token_rbracket)) && t.type <= token_string);
			}
			if(!stat_com()){
				if(SECOND_PASS) {
					if ((errno = sem_generate_jmp((op_t)while_label))) return errno;
					if ((errno = sem_generate_label())) return errno;
					sem_set_jmp_dst(while_jmp, (op_t)(active_function->instr_list_end));
				}
				return 0;
			}
		}
	}
	else if(is(token_k_return)){
		if(!ret_val() && is(token_semicolon)){
			if(SECOND_PASS) {
				if((errno = sem_generate_ret(precedence_result))) {
					return errno;
				}
			}
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
	if(is(token_id) || is(token_fqid) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return)){
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
		int err = 0;
		if(SECOND_PASS) {
			if ((errno = sem_generate_prepare((class_memb_t)sem_id_decoded.ptr))) return errno;
			
			if(sem_id_decoded.ptr == print_fn){ // in case of ifj16.print()
				int counter = 1;
				bool is_string = false;
				tok_que_t expr_queue = tok_que_init();
				token_t tmp;
				next_token();
				do{
					if((counter%2 == 0 && !is(token_addition)) || (counter%2 == 1 && is(token_addition))){
						fprintf(stderr, "ERR: Function ifj16.print() supports only simple concatenation expressions.\n");
						return 2;
					}
				
					if(is(token_addition))
						tok_enqueue(expr_queue, t);
					
					if((is(token_id) || is(token_fqid)) && !id()){
						if(((class_memb_t)sem_id_decoded.ptr)->dtype == dt_String)
							is_string = true;
						tmp.type = token_id;
						tmp.attr.p = sem_id_decoded.ptr;
						tok_enqueue(expr_queue, tmp);
					}
					else if(is(token_int) || is(token_double) || is(token_boolean) || is(token_string)){
						if(is(token_string))
							is_string = true;
						tmp.type = token_id;
						tmp.attr.p = add_global_helper_var(t, true); // insert as variable into TS
						if (!tmp.attr.p) return 99;
						tok_enqueue(expr_queue, tmp);
					}

					next_token();
					counter++;
				} while(is(token_id) || is(token_fqid) || is(token_int) || is(token_double) || is(token_string) || is(token_boolean) || is(token_addition));
				// if(!is_string){
				// 	printf("ERR: Incompatible types used as arguments of ifj16.print().\n");
				// 	return errno = 3; //TODO !!!IDK corrent return value.
				// }
				tmp.type = token_eof;
				tok_enqueue(expr_queue, tmp);
				err = precedence(expr_queue, &precedence_result);
				if(err)
					return errno = err;
				sem_rst_argcount();	
				op_t arg = precedence_result;
				if(precedence_result->dtype != dt_String) {
					arg = sem_generate_conv_to_str(precedence_result); // possible internal err
					if(!arg) return 99;
				}
				if ((errno = sem_generate_push(calling_function, arg))) return errno; // push result of concatenation

				if(is(token_rbracket))
					next_token();
				else
					return 2;
			}
			else
				err = fn_plist();

			if(is(token_semicolon)){					
				if(!sem_args_ok(calling_function)) {
					return errno = 4;
				}
				if ((errno = sem_generate_call(calling_function))) return errno;
				calling_function = NULL;
				return 0;
			}
			return err;			
		}
		else{ // FIRST_PASS
			do{
				next_token();
			} while(is(token_id) || is(token_fqid) || is(token_int) || is(token_double) || is(token_string) || is(token_boolean) || is(token_addition) || is(token_comma));
			if(is(token_rbracket)){
				next_token();
				if(is(token_semicolon)){
					return 0;
				}
			}
			return 2;
		}
	}
	else if(is(token_assign)) {
		op_t tmp_dst = NULL; // totally pointless, just to avoid optimizer warning
		if(SECOND_PASS) {
			tmp_dst = (op_t)(sem_id_decoded.ptr);
		}
		int err = assign();
		if(err == 0 && SECOND_PASS) {
			if(!calling_function) {
				if((err = sem_generate_mov(precedence_result, tmp_dst)))
					return errno = err;
			} else {
				if((err = sem_generate_movr(calling_function, tmp_dst)))
					return errno = err;
				calling_function = NULL;
			}
		}
		return err;
	}
	return 2;
}

int ret_val(){
	precedence_result = NULL;
	next_token();
	if(is(token_semicolon))
		return 0;
	tok_que_t expr_queue = tok_que_init(); // init new queue
	if(SECOND_PASS){
		if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
			return 2;
		if((errno = precedence(expr_queue, &precedence_result)))
			return errno;
		//semantic actions
	}
	else{
		do{ // in first pass skip the whole expression
			next_token();
		} while(t.type <= token_string);
	}
	return 0;
}

int _cond_fill_que(tok_que_t expr_queue, bool count_brackets){
	token_t tmp;
	int lb, rb;
	lb = rb = 0;
	do{
		if(t.type <= token_string && !is(token_id) && !is(token_fqid) && !is(token_int) && !is(token_double) && !is(token_boolean) && !is(token_string)){ // operators
			if(count_brackets){
				if(is(token_lbracket)) lb++;
				if(is(token_rbracket)) rb++;
			}
			tok_enqueue(expr_queue, t);
			next_token();
		}
		if(is(token_id) || is(token_fqid)){ // id processing
			if(!id()){
				tmp.type = token_id;
				tmp.attr.p = sem_id_decoded.ptr;
				tok_enqueue(expr_queue, tmp);
				next_token();
			}
			else
				return 2;
		}
		else if(is(token_int) || is(token_double) || is(token_boolean) || is(token_string)){ // literal processing
			tmp.type = token_id;
			tmp.attr.p = add_global_helper_var(t, true); // insert as variable into TS
			if(!tmp.attr.p) return 99;
			tok_enqueue(expr_queue, tmp);
			next_token();	
		}
	} while((count_brackets ? (lb != rb || is(token_rbracket)) : 1) && t.type <= token_string);
	tmp.type = token_eof; // add eof to the end of queue
	tok_enqueue(expr_queue, tmp);
	return 0;
}