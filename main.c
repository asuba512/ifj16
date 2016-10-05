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
	string_t string;
	string_t *s = &string;
	str_init(s);

	str_addstring(s, "pejsek");

	printf("%s\n", s->data);

	str_addstring(s, "a kocicka");

	printf("%s\n", s->data);
	
	str_addchar(s, '!');
	
	printf("%s\n", s->data);

	str_addstring(s, "Lorem ipsum dolor sit amet, consectetur adipiscing sed.");

	printf("%s\n", s->data);
//	get_token(fd);

//	fclose(fd);

    return 0;
}
