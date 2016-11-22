#include "token.h"
#include "infinite_string.h"
#include "precedence.h"
#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

int main(){
	
	token_t t;
	int a = 3;
	int b = 5;
	int c = 8;

	printf("3 5 8\n");

	tok_que_t q = tok_que_init();

	t.type = token_lbracket;
	tok_enqueue(q, t);

	t.type = token_id;
	t.attr.p = (void *)&a; 
	tok_enqueue(q, t);
	
	t.type = token_multiplication;
	tok_enqueue(q, t);

	t.type = token_id;
	t.attr.p = (void *)&b; 
	tok_enqueue(q, t);
	
	t.type = token_rbracket;
	tok_enqueue(q, t);

	t.type = token_and;
	tok_enqueue(q, t);

	t.type = token_id;
	t.attr.p = (void *)&c; 
	tok_enqueue(q, t);

	t.type = token_eof;
	tok_enqueue(q, t);

	printf("retval: %d\n", precedence(q));

    return 0;
}
