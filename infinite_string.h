#ifndef STR_INC
#define STR_INC 1

#define STRING_CHUNK 32
#define STRING_ERR 99
#define STRING_OK 0

typedef struct string {
	char *data;
	int max_length;
	int length;
} *string_t;

/* initializes string */
string_t str_init(const char *);

void str_destroy(string_t s);

/* adds single character into string */
int str_addchar(string_t s, char c);

/* concatenates two infinite strings */
int str_cat(string_t dest, string_t src);

/* concatenates infinite string with C-string */
int str_addstring(string_t s, const char *str);

/* compares two infinite strings */
int str_compare(string_t s1, string_t s2);

/* empties infinite string */
void str_empty(string_t s);

/* enlarges internal capacity of infinite string */
char *_str_add_chunk(string_t s, int chunk_count);
#endif
