#include "token.h"
#include "parser.h"
#include "scanner.h"

extern token_t t;
extern FILE *fd;
extern int lexerror;

#define next_token() !(lexerror = get_token(fd, t))
#define is(x) (t->type == x)

int c_list(){
	if(next_token() && t->type == token_k_class){
		if(next_token() && t->type == token_id){
			if(next_token() && t->type == token_lbrace){
				if(!memb_list() && t->type == token_rbrace){
					return c_list();
				}
			}
		}
	}
	else if(t->type == token_eof)
		return 0;
	return 2;
}

int memb_list(){
	if(next_token() && is(token_k_static)){
		return c_memb() || memb_list();
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
	if(next_token() && is(token_k_void)){
		if(next_token() && is(token_id)){
			if(next_token())
				return c_memb_func();
		}
	}
	else if(!type()){
		if(next_token() && is(token_id)){
			return c_memb2();
		}
	}
	return 2;
}

int type(){
	if(is(token_k_int) || is(token_k_double) || is(token_k_string) || is(token_k_boolean)){
		return 0;
	}
	return 2;
}

int c_memb_func(){
	if(is(token_lbracket)){
		if(!fn_def_plist() && is(token_rbracket)){
			if(next_token() && is(token_lbrace)){
				if(!fn_body() && is(token_rbrace)){
					return 0;
				}
			}
		}
	}
	return 2;
}

int c_memb2(){
	if(next_token() && is(token_assign)){
		do{
			if(!next_token())
				return 1;
		} while(!is(token_semicolon));
		return 0;
	}
	else if(is(token_semicolon)){
		return 0;
	}
	else if(is(token_lbracket)){
		return c_memb_func();
	}
	return 2;
}

int fn_def_plist(){
	if(next_token() && is(token_rbracket))
		return 0;
	else if(is(token_k_int) || is(token_k_double) || is(token_k_string) || is(token_k_boolean)){
		if(!par_def()){
			return fn_def_plist1();
		}
	}
	return 2;
}

int fn_def_plist1(){
	if(next_token() && is(token_comma)){
		if(next_token() && !par_def()){
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
		if(next_token() && is(token_id)){
			return 0;
		}
	}
	return 2;
}

int fn_body(){
	if(next_token() && (is(token_id) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return))){
		if(!stat()){
			return fn_body();
		}
	}
	else if(!type()){
		if(next_token() && is(token_id)){
			if(!opt_assign() && is(token_semicolon)){
				return fn_body();
			}
		}
	}
	else if(is(token_rbrace))
		return 0;
	return 2;
}

int opt_assign(){
	if(next_token() && is(token_assign)){
		return assign();
	}
	else if(is(token_semicolon)){
		return 0;
	}
	return 2;
}

/* HUGE MESS */
int assign(){
	if(next_token() && !id()){ // this will fail if "= ID;", usually there cannot be ';' after id, because we have "=E;" not "=ID;" in grammar
		// add token to queue
		if(is(token_lbracket)){
			//func call, can clear the queue
			if(!fn_plist()) //&& is(token_rbracket)){
				if(is(token_semicolon)) //next_token() && is(token_semicolon))
					return 0;
			}
	}
	else{ // this is mess... until we have precedence, it has to stay this way
		if(is(token_semicolon))
			return 0;
//		if(is(token_semicolon)) // temporary fix for "=ID;"
		do{
			if(!next_token()) //adding tokens to queue until semicolon
				return 1;
		} while(!is(token_semicolon));
		// here you would call precedence analysis
		return 0;
	}
	return 2;
}

int fn_plist(){
	if(next_token() && is(token_rbracket)){
		if(next_token())
			return 0;
	}
	else if(!val_id()){
		return fn_plist1();
	}
	return 2;
}

int fn_plist1(){
	if(is(token_rbracket)){
		if(next_token())
			return 0;
	}
	else if(is(token_comma)){
		if(next_token() && !val_id())
			return fn_plist1();
	}
	return 2;
}

int val_id(){
	if(!id()){
		return 0;
	}
	if(is(token_int) || is(token_double) || is(token_string) || is(token_boolean)){
		if(next_token())
			return 0;
	}
	return 2;
}

int id(){
	if(is(token_id)){
		return id1();
	}
	return 2;
}

int id1(){
	if(next_token() && is(token_dot)){
		if(next_token() && is(token_id)){
			if(next_token())
				return 0;	
		}
	}
	else if(is(token_lbracket) || is(token_rbracket) || is(token_assign) || is(token_comma)){
		return 0;
	}
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
		if(next_token() && is(token_lbracket)){
			lb++;
			do{
				if(!next_token())
					return 1;
				if(is(token_lbracket)) lb++;
				if(is(token_rbracket)) rb++;
			} while(lb != rb || !is(token_rbracket));
			lb = rb = 0;
			if(next_token() && !stat_com()){
				if(next_token() && is(token_k_else)){
					if(next_token() && !stat_com()){
						return 0;
					}
				}		
			}
		}
	}
	else if(is(token_k_while)){
		if(next_token() && is(token_lbracket)){
			lb++;
			do{
				if(!next_token())
					return 1;
				if(is(token_lbracket)) lb++;
				if(is(token_rbracket)) rb++;
			} while(lb != rb || !is(token_rbracket));
			lb = rb = 0;
			if(next_token() && !stat_com()){
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
	if(next_token() && (is(token_id) || is(token_lbrace) || is(token_k_if) || is(token_k_while) || is(token_k_return))){
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
	else if(is(token_assign)){
		return assign();
	}
	return 2;
}

int ret_val(){
	do{
		if(!next_token())
			return 1;
	} while(!is(token_semicolon));
	return 0;
}
