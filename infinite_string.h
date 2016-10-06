#ifndef STR_INC
#define STR_INC 1
/**
 * \file infinite_string.h
 * \brief Header file of library providing infinite strings in C.
 *
 */
#define STRING_CHUNK 32
#define STRING_ERR 1
#define STRING_OK 0

typedef struct string {
	char *data;
	int max_length;
	int length;
} *string_t;

string_t str_init(const char *);
void str_destroy(string_t s);

int str_addchar(string_t s, char c);
int str_cat(string_t dest, string_t src);
int str_addstring(string_t s, const char *str); // obsolete?

int str_compare(string_t s1, string_t s2);

char *_str_add_chunk(string_t s, int chunk_count);
#endif
