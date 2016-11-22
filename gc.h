#include <stdlib.h>

#define hash(x) (((long)x >> 5) %769)

typedef struct s_item{
	void *data;
	struct s_item *next;
} *s_item;

s_item ht[769];

void *gc_malloc(size_t s);
void *gc_realloc(void *, size_t);

void gc_push(int, void *);

void gc_remove(int, void*);

void free_all();
