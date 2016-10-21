#include "token.h"
#include "parser.h"
#include "scanner.h"

extern token_t t;
extern FILE *fd;

int clist(){
	get_token(fd, t);
	if(t->type == token_k_class){
		get_token(fd, t);
		if(t->type == token_id){
			get_token(fd, t);
			if(t->type == token_lbrace){
				if(cmemb() == 0){
					get_token(fd, t);
					if(t->type == token_rbrace){
						return clist();
					}
					else return 2;
				}
				else return 2;
			}
			else
				return 2;
		}
		else
			return 2;
	}
	else if(t->type == token_eof)
		return 0;
	else
		return 2;
}

int cmemb(){
	return 0;
}
