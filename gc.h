#include <stdlib.h>



typedef struct s_item{
	void *data;
	struct s_item *next;
} *s_item;

s_item ht[769];

void *mmaloc(size_t s);
void *rrealloc(void *, size_t);

void gc_push(int, void *);

void gc_remove(int, void*);

void free_all();
