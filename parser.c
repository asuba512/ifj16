#include "token.h"
#include "parser.h"
#include "scanner.h"

extern token_t t;
extern FILE *fd;
extern int lexerror;

int c_list(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_k_class){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			if(!(lexerror = get_token(fd, t)) && t->type == token_lbrace){
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

int c_memb(){
	if(t->type == token_k_static){
		return c_memb1();
	}	
	return 2;
}

int c_memb1(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_k_void){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			return c_memb_func();
		}
	}
	else if(!type()){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			return c_memb2();
		}
	}
	return 2;
}

int memb_list(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_k_static){
		return c_memb() || memb_list();
	}
	else if(t->type == token_rbrace){
		return 0;
	}
	return 2;
}

int type(){
	if(t->type == token_k_int || t->type == token_k_double || t->type == token_k_string || t->type == token_k_boolean)
		return 0;
	return 2;
}

int c_memb_func(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_lbracket){
		if(!fn_def_plist() && t->type == token_rbracket){
			if(!(lexerror = get_token(fd, t)) && t->type == token_lbrace){
				if(!fn_body() && t->type == token_rbrace){
					return 0;
				}
			}
		}
	}
	return 2;
}

int c_memb2(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_semicolon){
		return 0;
	}
	else if(t->type == token_lbracket){
		return c_memb_func();
	}
	else if(t->type == token_assign){
		return 0; //TODO EXP
	}
	return 2;
}

int fn_def_plist(){
	if(!(lexerror = get_token(fd, t)) && !par_def()){
		return fn_def_plist1();
	}
	else if(t->type == token_rbracket){
		return 0;
	}
	return 2;
}

int fn_def_plist1(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_comma){
		if(!(lexerror = get_token(fd, t)) && !par_def()){
			return fn_def_plist1();
		}
	}
	else if(t->type == token_rbracket){
		return 0;
	}
	return 2;
}

int par_def(){
	if(!type()){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			return 0;
		}
	}
	return 2;
}

int fn_body(){
	if(!(lexerror = get_token(fd, t)) && !stat() && !fn_body()){
		return 0;
	}
	else if(!type()){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			if(!(lexerror = get_token(fd, t)) && !opt_assign() && t->type == token_semicolon){
				return fn_body();
			}
		}
	}
	else if(t->type == token_rbrace){
		return 0;
	}
	return 2;
}

int stat_com(){
	if(t->type != token_lbrace){	
		if(!(lexerror = get_token(fd, t)) &&  !stat_list() && t->type == token_rbrace){
			return 0;
		}
	}
	return 2;
}

int stat_list(){
	if(t->type == token_rbrace){
		return 0;
	}
	else if(!stat()){
		if(!(lexerror = get_token(fd, t)) && t->type == token_rbrace){
			return 0;
		}
	}
	return 2;
}

int id(){
	if(t->type == token_id){
		return id1();
	}
	return 2;
}

int id1(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_dot){
		if(!(lexerror = get_token(fd, t)) && t->type == token_id){
			return 0;
		}
	}
	else if(t->type == token_lbracket || t->type == token_rbracket || t->type == token_assign || t->type == token_comma){
		return 0;
	}
	return 2;
}

int stat(){
	if(!id()){
		if(!as_ca() && !(lexerror = get_token(fd, t)) && t->type == token_semicolon){
			return 0;
		}
	}
	else if(t->type == token_lbrace){
		return stat_com();
	}
	else if(t->type == token_k_if){
		if(!(lexerror = get_token(fd, t)) && t->type == token_lbracket){
			///TODO EXPR
			if(!(lexerror = get_token(fd, t)) && t->type == token_rbracket){
				if(!(lexerror = get_token(fd, t)) && !stat_com()){
					if(!(lexerror = get_token(fd, t)) && t->type == token_k_else){
						if(!(lexerror = get_token(fd, t)) && !stat_com()){
							return 0;
						}
					}
				}
			}
		}
	}
	else if(t->type == token_k_while){
		if(!(lexerror = get_token(fd, t)) && t->type == token_lbracket){
			///TODO EXPR
			if(!(lexerror = get_token(fd, t)) && t->type == token_rbracket){
				if(!(lexerror = get_token(fd, t)) && !stat_com()){
					return 0;
				}
			}
		}
	}
	else if(t->type == token_k_return && !ret_val() && t->type == token_semicolon){
		return 0;
	}
	//TODO
	return 2;
}

int as_ca(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_lbracket){
		return fn_plist();
	}
	else if(t->type == token_assign){
		return assign();
	}
	return 2;
}

int opt_assign(){
	if(t->type == token_assign){
		return assign();
	}
	else if(t->type == token_semicolon){
		return 0;
	}
	return 2;
}

int assign(){
	if(!(lexerror = get_token(fd, t)) && !id()){ //TODO EXP
		//DEPENDING ON ID (FCE vs VAR) EXP MIGHT FOLLOW 
		if(!(lexerror = get_token(fd, t)) && t->type == token_lbracket){ // FUNCTION CALL
			if(!fn_plist() && t->type == token_rbracket){
				return 0;
			}
		}
	}
	return 2;
}

int ret_val(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_semicolon){
		return 0;
	}
	else if(1) { //TODO EXPR
		return 0;
	}
	return 2;
}

int fn_plist(){
	if(!(lexerror = get_token(fd, t)) && t->type == token_rbracket){
		return 0;
	}
	else if(!val_id()){
		return fn_plist1();
	}
	return 2;
}

int fn_plist1(){
	if(t->type == token_rbracket){
		return 0;
	}
	else if(t->type == token_comma){
		if(!(lexerror = get_token(fd, t)) && !val_id()){
			if(!(lexerror = get_token(fd, t)))
				return fn_plist1();
		}
	}
	return 2;
}

int val_id(){
	if(!id()){
		return 0;
	}
	else if(t->type == token_string || t->type == token_int || t->type == token_double || t->type == token_k_true || t->type == token_k_false){
		if(!(lexerror = get_token(fd, t)))
			return 0;
	}
	return 2;
}
