#ifndef IAL_INC
#define IAL_INC 1

#define BST_ERR -1
#define BST_NODE_ALREADY_EXISTS -2

#include <stdbool.h>
#include "infinite_string.h"

typedef struct bst_node {
	string_t key;
	void *data;
	struct bst_node *right_p, *left_p;
} *bst_node_t;

void BMA_compute_jumps(string_t substr, unsigned *char_jump);
int BMA(string_t str, string_t substr, unsigned *char_jump);

void shell_sort(string_t s);

bool bst_search(bst_node_t root, string_t key);
bst_node_t bst_search_get(bst_node_t root, string_t key);
int bst_insert(bst_node_t *root_ptr, string_t key, void *data);
int _bst_create_node(bst_node_t *node_ptr, string_t key, void *data);
void bst_delete(bst_node_t *root, string_t key);
void _bst_del(bst_node_t *node);
int bst_insert_or_err(bst_node_t *root_ptr, string_t key, void *data);

#endif
