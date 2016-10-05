#include "infinite_string.h"
#include <stdlib.h>
#include <string.h>

int str_init(string_t *s) {
	s->length = 0;
	s->mem_size = STRING_CHUNK;
	if((s->data = malloc(sizeof(char)*STRING_CHUNK)) == NULL)
		return STRING_ERR;
	s->data[0] = 0;
	return STRING_OK;
}

int str_addchar(string_t *s, char c) {
	if(s->length-1 == s->mem_size) {
		if((s->data = realloc(s->data, s->mem_size + STRING_CHUNK)) == NULL)
			return STRING_ERR;
		s->mem_size += STRING_CHUNK;
	}
	s->data[s->length++] = c;
	s->data[s->length] = 0;
	return STRING_OK;
}

void str_destroy(string_t *s) {
	free(s->data);
	s->data = NULL;
	s->length = -1;
	s->mem_size = -1;
}

int str_compare(string *s1, string *s2) {
	if(s1->length != s2->length) {
		return s1->length > s2->length ? 1 : 2;
	} else {
		return strcmp(s1->data, s2->data);
	}
}

int str_addstring(string *s, char *str) {
	return 0;
}