#ifndef IAL_INC
#define IAL_INC 1

#include <stdbool.h>
#include "infinite_string.h"

typedef struct bst_node {
	string_t key;
	int data;
	struct bst_node *right_p, *left_p;
} *bst_node_t;

bool bst_search(bst_node_t root, string_t key);
bst_node_t bst_search_get(bst_node_t root, string_t key);
void bst_insert(bst_node_t *root_ptr, string_t key, int data);
void _bst_create_node(bst_node_t *node_ptr, string_t key, int data);
void bst_delete(bst_node_t *root, string_t key);
void _bst_del(bst_node_t *node);

#endif