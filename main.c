/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "parser.h"
#include "token.h"
#include "infinite_string.h"
#include "sym_table.h"
#include "ial.h"
#include "ifj16_class.h"
#include "interpret.h"
#include "gc.h"
#include <stdio.h>
#include <stdlib.h>

extern string_t buff; // <- variable which has to be destroyed before exit, internal scanner variable
extern token_t t;
extern FILE *fd;
extern int errno;
extern int pass_number;
extern tok_que_t tok_q;

char *op[30] = {"halt", "add", "sub", "imul", "idiv", "conc", "eql", "neq", "gre", "less", "geq", "leq", "or", "and", "not", "mov", "i_dbl",
				"i_str", "b_str", "d_str", "sframe", "call", "label", "jmp", "jmpifn", "jmpif", "push", "ret", "movr", "idr"};

int add_head();

int main(int argc, char **argv){
	(void)argc;
	fd = fopen(argv[1],"r");
	if(fd == NULL){
		printf("error\n");
		return 99;
	}
	init_class_table();
	populate_sym_table();
	tok_q = tok_que_init();
	pass_number = 1;
	int retval = c_list();
	fclose(fd);
	//printf("1st pass retval: %d (parser) %d (errno)\n", retval, errno);
	if(errno) {
		free_all();
		st_destroy_all();
		if(errno == 1)
			fprintf(stderr, "ERR: Lexical error.\n");
		return errno;
	}
	if(retval) {
		free_all();
		st_destroy_all();
		fprintf(stderr, "ERR: Syntax error.\n");
		return retval;
	}
	// printf("current token: %d\n", t.type);
	// if(retval != 0){	
	// 	int c;
	// 	while((c = getc(fd)) != EOF){
	// 		putchar(c);
	// 	}
	// 	printf("ERR: First pass failed.\n");
	// 	return 9999999;
	// }
	pass_number = 2;
	retval = c_list();
	//printf("2nd pass retval: %d (parser) %d (errno)\n", retval, errno);
	if(errno) {
		free_all();
		st_destroy_all();
		return errno;
	}
	if(retval) {
		free_all();
		st_destroy_all();
		fprintf(stderr, "ERR: Syntax error.\n");
		return retval;
	}
	// printf("current token: %d\n", t.type);
	
	str_destroy(buff);
	if((errno = add_head())) {
		free_all();
		st_destroy_all();
		return errno;
	}
	// printf("Global instruction tape:\n");
 //instr_t i = glob_instr_list.head;
	// for (instr_t ins = i; ins != NULL; ins = (instr_t)ins->next) {
	// 	printf("%s\t%p, %p, %p\n", op[ins->type], (void*)ins->src1, (void*)ins->src2, (void*)ins->dst);
	// }
	//printf("\nMain.run(): \n");
 //i = (instr_t)(st_getmemb(st_getclass(str_init("Main")), str_init("run"))->instr_list);
 /*for (instr_t ins = i; ins != NULL; ins = (instr_t)ins->next) {
 	printf("%s\t%p, %p, %p\n", op[ins->type], (void*)ins->src1, (void*)ins->src2, (void*)ins->dst);
 }
*/
	/// START INTERPRETATION HERE
	int a = inter(glob_instr_list.head);
	//printf("\nInterpret ret val: %d\n", a);
	free_all();
	st_destroy_all();
    return a;
}

int add_head() {
	struct instr i;
	i.type = sframe;
	class_t c = st_getclass(str_init("Main"));
	if(!c) {
		fprintf(stderr,"ERR: Missing 'Main' class.\n");
		return 3;
	}
	i.src1 = (op_t)st_getmemb(c, str_init("run"));
	if(!i.src1) {
		fprintf(stderr,"ERR: Missing 'Main.run()' function.\n");
		return 3;
	}
	if(((class_memb_t)(i.src1))->dtype != t_void) {
		fprintf(stderr,"ERR: 'Main.run()' must be a void-funcion.\n");
		return 4;
	}
	i.dst = i.src2 = NULL;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}
	i.dst = i.src1;
	i.src1 = NULL;
	i.type = call;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}	
	i.type = label;
	i.dst = i.src1 = i.src2 = NULL;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}
	return 0;
}
