#include "infinite_string.h"
#include <stdlib.h>
#include <string.h>
#include "gc.h"

string_t str_init(const char *str) {
	string_t s;
	if((s = gc_malloc(sizeof(struct string))) == NULL)
		return NULL;
	int str_l = strlen(str);
	int k = (str_l / STRING_CHUNK) + 1; // number of chunks to be allocated
	s->length = str_l; // actual length of string
	s->max_length = STRING_CHUNK * k; // capacity of string
	if((s->data = gc_malloc(sizeof(char) * (STRING_CHUNK * k + 1))) == NULL){
		return NULL;
	}
	strcpy(s->data, str); // store actual data
	return s;
}

void str_destroy(string_t s) {
	(void)s;
}

int str_addchar(string_t s, char c) {
	if(s->length == s->max_length) { // in case you need to expand, expand only by 1 chunk
		if((s->data = _str_add_chunk(s, 1)) == NULL)
			return STRING_ERR;
	}
	s->data[s->length++] = c;
	s->data[s->length] = 0;
	return STRING_OK;
}

/* concatenation of two infinite string */
int str_cat(string_t dest, string_t src) {
	int char_difference = dest->max_length - dest->length - src->length; // calculates how many free characters are left after concatenation
	if(char_difference >= 0) {
		strncpy(dest->data + dest->length, src->data, src->length + 1);
	} else { // in case of unsufficient capacity
		if((dest->data = _str_add_chunk(dest, (-char_difference) / STRING_CHUNK + 1)) == NULL) // calculates exact amount of chunks to add
			return STRING_ERR;
		strncpy(dest->data + dest->length, src->data, src->length + 1);
	}
	dest->length += src->length;
	return STRING_OK;
}

/* concatenation of infinite string and C-string */
int str_addstring(string_t s, const char *str) {
	int char_difference = (s->max_length - s->length) - strlen(str); // calulates how many free characters are left after concatenation
	if(char_difference >= 0) {
		strcpy(s->data + s->length, str);
	} else { // in case of unsufficient capacity
		if((s->data = _str_add_chunk(s, (-char_difference) / STRING_CHUNK + 1)) == NULL) // calculates exact amount of chunks to add
			return STRING_ERR;
		strcpy(s->data + s->length, str);
	}
	s->length += strlen(str);
	return STRING_OK;
}

/* encapsulating function for comparison */
int str_compare(string_t s1, string_t s2) {
		return strcmp(s1->data, s2->data);
}

void str_empty(string_t s){
	s->data[0] = 0;
	s->length = 0;
}

/* adds capacity for another 'chunk_count' chunks */
char *_str_add_chunk(string_t s, int chunk_count) {
	s->max_length += chunk_count*STRING_CHUNK;
	return gc_realloc(s->data, s->max_length + 1);
}
