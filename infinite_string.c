#include "infinite_string.h"
#include <stdlib.h>
#include <string.h>

int str_init(string_t *s) {
	s->length = 0;
	s->max_length = STRING_CHUNK;
	if((s->data = malloc(sizeof(char) * (STRING_CHUNK + 1))) == NULL)
		return STRING_ERR;
	s->data[0] = 0;
	return STRING_OK;
}

int str_addchar(string_t *s, char c) {
	if(s->length == s->max_length) {
		if((s->data = _str_add_chunk(s, 1)) == NULL)
			return STRING_ERR;
	}
	s->data[s->length++] = c;
	s->data[s->length] = 0;
	return STRING_OK;
}

void str_destroy(string_t *s) {
	free(s->data);
	s->data = NULL;
	s->length = -1;
	s->max_length = -1;
}

int str_compare(string_t *s1, string_t *s2) {
	if(s1->length != s2->length) {
		return s1->length > s2->length ? 1 : 2;
	} else {
		return strcmp(s1->data, s2->data);
	}
}

int str_addstring(string_t *s, const char *str) {
	int char_difference = (s->max_length - s->length) - strlen(str);
	if(char_difference >= 0) {
		strcpy(s->data + s->length, str);
	} else {
		if((s->data = _str_add_chunk(s, (-char_difference) / STRING_CHUNK + 1)) == NULL)
			return STRING_ERR;
		strcpy(s->data + s->length, str);
	}
	s->length += strlen(str);
	return STRING_OK;
}

char *_str_add_chunk(string_t *s, int chunk_count) {
	s->max_length += chunk_count*STRING_CHUNK;
	return realloc(s->data, s->max_length);
}
