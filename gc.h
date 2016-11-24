#ifndef HT
#define HT 1

#include <stdlib.h>

#define hash(x) (((long)x >> 5) %769)

/*
* Elements of linked list
*/
typedef struct s_item{
	void *data;
	struct s_item *next;
} *s_item;

/*
* Hash table with explicit linking of synonyms
*/
s_item ht[769];

/*
* Malloc new memory of size s
*/
void *gc_malloc(size_t s);

/*
* Reallocs memory from pointer.
* If new pointer is returned from realloc, it removes old pointer from hashtable
*/
void *gc_realloc(void *, size_t);

/*
* Pushes new pointer to hash table
*/
int gc_push(int, void *);

/*
* Removes passed pointer from hash table
*/
void gc_remove(int, void*);

/*
* Free's all allocated memory
*/
void free_all();

#endif