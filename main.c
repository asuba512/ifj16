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
#include "semantic_analysis.h"
#include <stdio.h>
#include <stdlib.h>

extern token_t t;
extern FILE *fd;
extern int error_number;
extern int pass_number;
extern tok_que_t tok_q;

int main(int argc, char **argv){
	/* Opening file and initialization */
	(void)argc;
	fd = fopen(argv[1],"r");
	if(fd == NULL){
		printf("ERR: Cannot open source file\n");
		return 99;
	}
	init_class_table();
	populate_sym_table(); //  adds class ifj16
	tok_q = tok_que_init();

	/* FIRST PASS */
	pass_number = 1;
	int retval = c_list();
	fclose(fd);

	if(error_number) { // errors except syntax err
		free_all();
		st_destroy_all();
		if(error_number == 1)
			fprintf(stderr, "ERR: Lexical error.\n");
		return error_number;
	}
	if(retval) { // syntax err
		free_all();
		st_destroy_all();
		fprintf(stderr, "ERR: Syntax error.\n");
		return retval;
	}

	/* SECOND PASS */
	pass_number = 2;
	retval = c_list();
	if(error_number) { // errors except syntax err
		free_all();
		st_destroy_all();
		if(error_number == 2)
			fprintf(stderr, "ERR: Syntax error.\n");
		return error_number;
	}
	if(retval) { // syntax err
		free_all();
		st_destroy_all();
		fprintf(stderr, "ERR: Syntax error.\n");
		return retval;
	}	

	str_destroy(buff);
	if((error_number = add_head())) { // calls Main.run at the end of global instr. tape
		free_all();
		st_destroy_all();
		return error_number;
	}

	/* INTERPRETATION */
	retval = inter(glob_instr_list.head);
	free_all();
	st_destroy_all();
	
    return retval;
}

