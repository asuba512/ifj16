#include "token.h"
#include "parser.h"
#include "scanner.h"
#include "semantic_analysis.h"
#include "sym_table.h"
#include "infinite_string.h"
#include "precedence.h"
#include "ifj16_class.h"
#include <string.h>

extern struct temp_data sem_tmp_data;
extern struct fq sem_id_decoded;
extern class_memb_t calling_function;
extern class_memb_t print_fn;
extern op_t precedence_result;
extern bool outside_func;

#define next_token() do{ if(FIRST_PASS){ if((error_number = get_token(fd, &t)) ) return 2; tok_enqueue(tok_q, t); } else { t = tok_q->head->tok; tok_remove_head(tok_q); }} while(0)
#define is(x) (t.type == x)

int c_list(){
	// initialize global variables used later
	outside_func = true;
	sem_id_decoded.ptr = NULL;
	calling_function = NULL;
	next_token();
	if(is(token_k_class)){
		next_token();
		if(is(token_id)){
			if (FIRST_PASS) {
				error_number = sem_new_class(t.attr.s); // first pass adds this class into ST
				if (error_number) return error_number;
			}
			else if(SECOND_PASS) {
				sem_set_active_class(t.attr.s); // second pass marks this class as active scope for searching members
			}
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
	active_function = NULL; // we're not in a function yet
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
		// reached some definition
		if(FIRST_PASS)
			sem_tmp_data.dt = t_void; // remember datatype
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s; // remember identifier
			next_token();
			return c_memb_func();
		}
	}
	else if(!type()){
		next_token();
		if(is(token_id)){
			sem_tmp_data.id = t.attr.s; // remember identifier
			return c_memb2();
		}
	}
	return 2;
}

int type(){
	// if there's a datatype token, remember it, so it can be used later to insert symbol into ST
	if(is(token_k_int)){
		if(FIRST_PASS) sem_tmp_data.dt = dt_int;
		return 0;
	} else if (is(token_k_double)) {
		if(FIRST_PASS) sem_tmp_data.dt = dt_double;
		return 0;
	} else if(is(token_k_string)) {
		if(FIRST_PASS) sem_tmp_data.dt = dt_String;
		return 0;
	} else if(is(token_k_boolean)){
		if(FIRST_PASS) sem_tmp_data.dt = dt_boolean;
		return 0;
	}
	return 2;
}

int c_memb_func(){
	if(is(token_lbracket)){
		if (FIRST_PASS) {
			error_number = sem_add_member_active_class(func); // first pass adds function to symbol table
			if (error_number) return error_number;
		}
		else if (SECOND_PASS)
			sem_set_active_fn(t.attr.s); // second pass marks this function as active local scope (for searching purposes)
		if(!fn_def_plist() && is(token_rbracket)){
			next_token();
			if(is(token_lbrace)){
				outside_func = false; // we're entering body of a function
				if(!fn_body() && is(token_rbrace)){
					return 0; // ('active_function' and 'outside_fun'c have been already reset)
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
			// last token was assignment, add new symbol as variable
			if((error_number = sem_add_member_active_class(var))) return error_number;
			// and skip the expression for now
			do{
				next_token();
				if(is(token_eof)) return 2;
			} while(!is(token_semicolon));
		} else {
			sem_mark_sec_pass(sem_tmp_data.id); // this variable can now be used in static initializers
			class_memb_t tmp_dst = st_getmemb(active_class, sem_tmp_data.id);
			tok_que_t expr_queue = tok_que_init();
			// prepare precedence analysis
			next_token();
			if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
				return 2;
			error_number = precedence(expr_queue, &precedence_result); // evaluate expression
			if(error_number == 2) {
				error_number = 0;
				return 2;
			}
			if(!error_number) {
				if((error_number = sem_generate_mov(precedence_result, (op_t)tmp_dst)))
					return error_number;
			}
			return error_number;
		}
		return 0;
	}
	else if(is(token_semicolon)){
		if (FIRST_PASS) {
			// symbol was definitely a variable, insert into ST
			if ((error_number = sem_add_member_active_class(var))) return error_number;
		}
		else {
			sem_mark_sec_pass(sem_tmp_data.id); // this variable can now be used in static initializers
		}			
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
				// we've just collected type and id of argument, let's submit that into ST
				error_number = sem_add_arg_active_fn();
				if(error_number) return error_number;
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
				// submitting new local variable into ST using collected information
				error_number = sem_new_loc_var(sem_tmp_data.dt, t.attr.s);
				if(error_number) return error_number;
				// remembers pointer to just submitted var in sem_id_decoded.ptr so we can use it as destination of initializer assignment later
				sem_search(NULL, t.attr.s); 
			}
			if(!opt_assign() && is(token_semicolon)){
				return fn_body();
			}
		}
	}
	else if(is(token_rbrace)) {
		// leaving function body
		if(SECOND_PASS) {
			if(active_function->op.dtype == t_void) {
				// append ret instruction at the end of void function automatically
				if ((error_number = sem_generate_ret(NULL))) return error_number; 
			}
			else {
				// append halt instruction at the end of non-void function. proper funcion should not reach this instruction
				if ((error_number = sem_generate_halt())) return error_number; 
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
			tmp_dst = (op_t)(sem_id_decoded.ptr); // remembers pointer to just defined variable in fn_body()
		}
		// produces result of expression evaluation in 'precedence_result' variable (assignment by evaluating expression)
		// or sets pointer to called function (assignment by function call)
		int err = assign(); 
		if(err == 0 && SECOND_PASS) {
			// generate either regular assignment instruction or assignment from return value of function
			if(!calling_function) { // if 'calling_function' isn't NULL, we're assigning return value
				if((error_number = sem_generate_mov(precedence_result, tmp_dst)))
					return error_number;
			} else {
				if((error_number = sem_generate_movr(calling_function, tmp_dst)))
					return error_number;
				calling_function = NULL; // make sure that the value is not reused in future
			}
		}
		return err;
	}
	else if(is(token_semicolon)){
		return 0;
	}
	return 2;
}

/** Determines what is going to be assigned and sets necessary values. */
int assign(){
	if(FIRST_PASS){ 
		// we pretty much know nothing in first pass, skipping everything
		do{
			next_token();
		} while(t.type <= token_string || is(token_comma)); // skip commas, too, there could be a func call
		return 0;
	}
	if(SECOND_PASS){ 
		next_token();
		// in this place, we use id() to check: IF token IS id/fqid THEN it MUST BE defined
		// (the program is ok if and only if this implication is true)
		if(!id() && sem_id_decoded.isFun){
			// identifier token after '=' belongs to func, we try to call that function
			if(((class_memb_t)sem_id_decoded.ptr)->op.dtype == t_void) {
				fprintf(stderr, "ERR: Cannot assign return value of void-function.\n");
				return error_number = 8;
			}
			if((error_number = sem_generate_prepare((class_memb_t)sem_id_decoded.ptr))) return error_number; // sframe instruction
			next_token();
			if(is(token_lbracket)){
				if(!fn_plist()) 
					if(is(token_semicolon)){					
						if(!sem_args_ok(calling_function)) {
							return error_number = 4; // checks whether the number of given args is ok
						}
						if((error_number = sem_generate_call(calling_function))) return error_number; // finally generate call instruction
						// 'movr' is generated outside assign() nonterminal
						return 0;
					}
			}
		}
		else if(error_number) {
			// token in id() WAS is/fqid and WAS NOT defined -> error
			return error_number;
		}
		else{ // expression [(token WAS NOT an id -> NOT AN ERROR) OR (token WAS defined id/fqid and was belongs to variable)]
			token_t tmp;
			tok_que_t expr_queue = tok_que_init();
			if(sem_id_decoded.ptr){ // if there was id as first token
				tmp.type = token_id;
				tmp.attr.p = sem_id_decoded.ptr;
				tok_enqueue(expr_queue, tmp);
				next_token(); // _cond_fill_que expects first token to be already retrieved
			}
			else if(t.type > token_string){ // unsupported tokens
				return 2;				
			}
			if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
				return 2;
			return error_number = precedence(expr_queue, &precedence_result); // finally evaluate the expession
			// 'mov' is generated outside assign() nonterminal
		}
	}
	return 2;
}

int fn_plist(){
	next_token();
	if(SECOND_PASS)
		sem_rst_argcount(); // reset counter of processed args
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
	// processing function arguments
	if(!id()){
		if(SECOND_PASS) {
			if(calling_function) {
				// if token was identifier (definition check was already performed), then generate 'push' instruction
				if ((error_number = sem_generate_push(calling_function, (op_t)sem_id_decoded.ptr))) return error_number;
			}
		}
		return 0;
	}
	if(is(token_int) || is(token_double) || is(token_string) || is(token_boolean)){
		if(SECOND_PASS) {
			if(calling_function) {
				// if token was literal, then we need to add it into list of helper variables, then generate 'push'
				op_t literal = (op_t)add_global_helper_var(t, true);
				if ((error_number = sem_generate_push(calling_function, literal))) return error_number;
			}
		}
		return 0;
	}
	return 2;
}

int id(){
	/** In second pass, this nonterminal tries to search for the indentifier in symbols table.
	 	In case of success, sem_id_decoded.ptr points to found symbol in ST (and can be used
	 	in semantic analysis later) */
	if(is(token_id)){
		if(SECOND_PASS) {			
			sem_search(NULL, t.attr.s); // search for symbol in implicit (active) class
			if(!sem_id_decoded.ptr) {
				fprintf(stderr, "ERR: Unknown identifier %s\n", t.attr.s->data);
				return error_number = 3;
			} else if(outside_func && ((class_memb_t)(sem_id_decoded.ptr))->type == var && !(((class_memb_t)(sem_id_decoded.ptr))->second_pass)) {
				// the case when identifier IS in ST, but cannot be used yet, because it is defined lexically later
				fprintf(stderr, "ERR: Static var must be defined before its usage.\n");
				return error_number = 6;
			}
		}
		return 0;
	}
	else if(is(token_fqid)){
		string_t class_id = NULL, memb_id = NULL;
		if(SECOND_PASS) {
			// split fully qualified identifier into 'class part' and 'member part' (fqid)
			char *ptr = strchr(t.attr.s->data, '.');
			*ptr = 0;
			if(!(class_id = str_init(t.attr.s->data))) {
				fprintf(stderr, "ERR: Internal error.\n");
				return error_number = 99;
			}
			if(!(memb_id = str_init(ptr + 1))) {
				fprintf(stderr, "ERR: Internal error.\n");
				return error_number = 99;
			}

			sem_search(class_id, memb_id); // search for symbol in explicit class
			if(!sem_id_decoded.ptr) {
				fprintf(stderr, "ERR: Unknown identifier %s.%s\n", t.attr.s->data, ptr + 1);
				return error_number = 3;
			} else if(outside_func && ((class_memb_t)(sem_id_decoded.ptr))->type == var && !(((class_memb_t)(sem_id_decoded.ptr))->second_pass)) {
				// the case when identifier IS in ST, but cannot be used yet, because it is defined lexically later
				fprintf(stderr, "ERR: Static var must be defined before its usage.\n");
				return error_number = 6;
			}
		}
		return 0;
	}
	if(SECOND_PASS)
		sem_id_decoded.ptr = NULL;
	return 2;
}

int stat(){
	int lb = 0, rb = 0; // helper variables for counting brackets
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
				if((error_number = precedence(expr_queue, &precedence_result)))
					return 2;
				if((error_number = sem_generate_jmpifn(precedence_result))) return error_number;
				if_jmp_instr = active_function->instr_list_end; // retrieve newly created conditional jump instr. (we'll add a target later)
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
						if ((error_number = sem_generate_jmp(NULL))) return error_number; // jump to skip the 'else' part
						pre_else_jmp = active_function->instr_list_end; // target will be added later
						if ((error_number = sem_generate_label())) return error_number;
						//set label we just created as target to this 'else' part
						sem_set_jmp_dst(if_jmp_instr, (op_t)(active_function->instr_list_end)); 
					}					
					next_token();
					if(!stat_com()){
						if(SECOND_PASS) {
							if ((error_number = sem_generate_label())) return error_number;
							sem_set_jmp_dst(pre_else_jmp, (op_t)(active_function->instr_list_end)); // target of jmp that skips 'else' part
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
			if ((error_number = sem_generate_label())) return error_number;
			while_label = active_function->instr_list_end;
			// remember label as target of looping jump that will be created at the end of while statement
		}
		next_token();
		if(is(token_lbracket)){
			tok_que_t expr_queue = tok_que_init(); // init new queue
			if(SECOND_PASS){
				if(_cond_fill_que(expr_queue, true)) // fill queue, counting brackets is on
					return 2;
				if((error_number = precedence(expr_queue, &precedence_result)))
					return error_number;
				// generate conditional jump (that ends looping)
				if ((error_number = sem_generate_jmpifn(precedence_result))) return error_number;
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
					// jump instruction to the part that re-evaluates while loop condition 
					if ((error_number = sem_generate_jmp((op_t)while_label))) return error_number;
					// target label of conditional jump that sits after evaluating loop condition sequence
					if ((error_number = sem_generate_label())) return error_number;
					sem_set_jmp_dst(while_jmp, (op_t)(active_function->instr_list_end));
				}
				return 0;
			}
		}
	}
	else if(is(token_k_return)){
		if(!ret_val() && is(token_semicolon)){ // ret_val() generates instructions from return statement expression (if provided)
			if(SECOND_PASS) {
				// generate return instruction returning result of precedence analysis ('precedence_result' is NULL if no expr. was provided)
				if((error_number = sem_generate_ret(precedence_result))) { 
					return error_number;
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

int as_ca() {
	if(is(token_lbracket)){
		// function call
		int err = 0;
		if(SECOND_PASS) {
			if ((error_number = sem_generate_prepare((class_memb_t)sem_id_decoded.ptr))) return error_number; // sframe
			if(sem_id_decoded.ptr == print_fn){ // in case of ifj16.print()
				// we're going to evaluate simple concatenation and push the result as function argument like any other
				// this part pretty much simulates the behaviour of fn_plist nonterminal, but allows concatenation
				int counter = 1;
				bool is_string = false;
				tok_que_t expr_queue = tok_que_init();
				token_t tmp;
				next_token();
				do{
					if(is(token_comma)){
						fprintf(stderr, "ERR: Function ifj16.print() takes only simple concatenation expression.\n");
						return 2;
					}
					if((counter%2 == 0 && !is(token_addition)) || (counter%2 == 1 && is(token_addition))){
						fprintf(stderr, "ERR: Function ifj16.print() supports only simple concatenation expressions.\n");
						return 2;
					}
					if(is(token_addition))
						tok_enqueue(expr_queue, t);
					
					if((is(token_id) || is(token_fqid)) && !id()){
						if(((class_memb_t)sem_id_decoded.ptr)->op.dtype == dt_String)
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
					if(error_number) return 3; // identifier was undefined
					next_token();
					counter++;
				} while(is(token_id) || is(token_fqid) || is(token_int) || is(token_double) || is(token_string) || is(token_boolean) || is(token_addition) || is(token_comma));

				if(!(counter == 2 && is(token_rbracket)) && !is_string){
				 	fprintf(stderr, "ERR: Incompatible types used as arguments of ifj16.print().\n");
				 	return error_number = 4;
				}

				tmp.type = token_eof;
				tok_enqueue(expr_queue, tmp);
				err = precedence(expr_queue, &precedence_result);
				if(err)
					return error_number = err;
				sem_rst_argcount();	// reset counter of pushed arguments, we're processing function call like any other
				op_t arg = precedence_result;
				if(precedence_result->dtype != dt_String) {
					// handles cases like ifj16.print(42), ifj16.print(42.0), ifj16.print(false) etc.
					arg = sem_generate_conv_to_str(precedence_result);
					if(!arg) return 99;
				}
				if ((error_number = sem_generate_push(calling_function, arg))) return error_number; // push result of concatenation

				if(is(token_rbracket))
					next_token();
				else
					return 2;
			}
			else // calling something else (not ifj16.print)
				err = fn_plist(); // process arguments regularly

			// performing standard check after processing arguments, this part applies to EVERY call (even ifj16.print)
			if(is(token_semicolon)){					
				if(!sem_args_ok(calling_function)) {
					return error_number = 4;
				}
				if ((error_number = sem_generate_call(calling_function))) return error_number;
				calling_function = NULL;
				return 0;
			}
			return err;			
		}
		else{ // FIRST_PASS
			// skip contents the contents of parentheses part  - foo( ... ), will be processed in second pass
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
		// assigment
		op_t tmp_dst = NULL; // initialization is totally pointless, just to avoid optimizer warning
		if(SECOND_PASS) {
			tmp_dst = (op_t)(sem_id_decoded.ptr); // remember destination of assignment
		}
		int err = assign(); // evaluates expression or assignment by function call
		if(err == 0 && SECOND_PASS) {
			// generate assignment instruction
			if(!calling_function) { // if calling_function == NULL, generate assigment from result of expression analysis
				if((err = sem_generate_mov(precedence_result, tmp_dst)))
					return error_number = err;
			} else { // if calling_function != NULL, generate assignment from return value of function 
				if((err = sem_generate_movr(calling_function, tmp_dst)))
					return error_number = err;
				calling_function = NULL; // prevent accidental global variable reusage
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
	if(SECOND_PASS){ // second pass processes the expression
		if(_cond_fill_que(expr_queue, false)) // fill queue, counting brackets is off
			return 2;
		if((error_number = precedence(expr_queue, &precedence_result)))
			return error_number;
		// return instruction is generated outside ret_val nonterminal
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
			if(!tmp.attr.p) {
				fprintf(stderr, "ERR: Internal error.\n");
				return 99;
			}
			tok_enqueue(expr_queue, tmp);
			next_token();	
		}
	} while((count_brackets ? (lb != rb || is(token_rbracket)) : 1) && t.type <= token_string);
	tmp.type = token_eof; // add eof to the end of queue
	tok_enqueue(expr_queue, tmp);
	return 0;
}
