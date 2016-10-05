#define STRING_CHUNK 32
#define STRING_ERR 1
#define STRING_OK 0

typedef struct string {
	char *data;
	int max_length;
	int length;
} string_t;

int str_init(string_t *s);
int str_addchar(string_t *s, char c);
void str_destroy(string_t *s);
int str_compare(string_t *s1, string_t *s2);
int str_addstring(string_t *s, const char *str);
string_t *_str_add_chunk(string_t *s, int chunk_count);