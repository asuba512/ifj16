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

	str_addstring(s, "pejsek a kocicka");

	printf("%d: %s\n", s->max_length, s->data);

	str_addstring(s, "pejsek a kocicka");

	printf("%d: %s\n", s->max_length, s->data);
	
	str_addchar(s, '!');
	printf("%d: %s\n", s->max_length, s->data);
	str_addchar(s, '!');
	printf("%d: %s\n", s->max_length, s->data);
	str_addchar(s, '!');
	
	printf("%d: %s\n", s->max_length, s->data);

	str_addstring(s, "Lorem ipsum dolor sit amet, consectetur adipiscing sed.");

	printf("%d: %s\n", s->max_length, s->data);

	str_addstring(s, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean ac luctus arcu. Nam faucibus neque feugiat malesuada porta. Curabitur placerat nullam.");
	printf("%d: %s\n", s->max_length, s->data);
//	get_token(fd);

//	fclose(fd);

    return 0;
}
