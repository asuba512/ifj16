/**
 * \file main.c
 * \brief Main module of IFJ16 programming language interpreter.
 *
 */

#include "scanner.h"
#include <stdio.h>

int main(){

	FILE *fd = fopen("zdroj.ifj","r");
	if(fd == NULL){
		printf("error\n");
		return 1;
	}

	get_token(fd);

	fclose(fd);

    return 0;
}
