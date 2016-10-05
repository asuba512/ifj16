/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include "infinite_string.h"
#include <stdio.h>

int main(){

/*	FILE *fd = fopen("zdroj.ifj","r");
	if(fd == NULL){
		printf("error\n");
		return 1;
	}
*/


	string_t a[11];
	a[0] = str_init("for");
	a[1] = str_init("while");
	a[2] = str_init("else");
	a[3] = str_init("int");
	a[4] = str_init("double");
	a[5] = str_init("string");
	a[6] = str_init("return");
	a[7] = str_init("switch");
	a[8] = str_init("case");
	a[9] = str_init("break");
	a[10] = str_init("continue");

	string_t b = str_init("fe");

	for(int i = 0; i < 30000000; i++){
		for(int j = 0; j < 11; j++)
			if(str_compare(a[j], b) == 0)
			break;
	}

	for(int i = 0; i < 11; i++)
		str_destroy(a[i]);

	str_destroy(b);

//	get_token(fd);

//	fclose(fd);

    return 0;
}
