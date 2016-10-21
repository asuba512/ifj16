/**
 * \file infinite_string.c
 * \brief Library providing infinite strings in C.
 *
 */
#include "infinite_string.h"
#include <stdlib.h>
#include <string.h>

string_t str_init(const char *str) {
	string_t s;
	if((s = malloc(sizeof(struct string))) == NULL)
		return NULL;
	int str_l = strlen(str);
	int k = (str_l / STRING_CHUNK) + 1;
	s->length = str_l;
	s->max_length = STRING_CHUNK * k;
	if((s->data = malloc(sizeof(char) * (STRING_CHUNK * k + 1))) == NULL){
		free(s);
		return NULL;
	}
	strcpy(s->data, str);
	return s;
}

void str_destroy(string_t s) {
	free(s->data);
	free(s);
}

int str_addchar(string_t s, char c) {
	if(s->length == s->max_length) {
		if((s->data = _str_add_chunk(s, 1)) == NULL)
			return STRING_ERR;
	}
	s->data[s->length++] = c;
	s->data[s->length] = 0;
	return STRING_OK;
}

int str_cat(string_t dest, string_t src) {
	int char_difference = dest->max_length - dest->length - src->length;
	if(char_difference >= 0) {
		strncpy(dest->data + dest->length, src->data, src->length + 1);
	} else {
		if((dest->data = _str_add_chunk(dest, (-char_difference) / STRING_CHUNK + 1)) == NULL)
			return STRING_ERR;
		strncpy(dest->data + dest->length, src->data, src->length + 1);
	}
	dest->length += src->length;
	return STRING_OK;
}

int str_addstring(string_t s, const char *str) {
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

int str_compare(string_t s1, string_t s2) {
		return strcmp(s1->data, s2->data);
}

void str_empty(string_t s){
	s->data[0] = 0;
	s->length = 0;
}

char *_str_add_chunk(string_t s, int chunk_count) {
	s->max_length += chunk_count*STRING_CHUNK;
	return realloc(s->data, s->max_length + 1);
}
