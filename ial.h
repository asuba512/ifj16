#ifndef IAL_INC
#define IAL_INC 1

#define BST_MEM_ERR 99 // internal error code
#define BST_NODE_ALREADY_EXISTS 3 // trying to redefine something

#include <stdbool.h>
#include "infinite_string.h"

typedef struct bst_node {
	string_t key;
	void *data; // effective data of BST node is a void pointer, because we store multiple types of structures in it
	struct bst_node *right_p, *left_p;
} *bst_node_t;

void BMA_compute_jumps(string_t substr, unsigned *char_jump);
int BMA(string_t str, string_t substr, unsigned *char_jump);
int BMA_index(string_t str,string_t substr);

void shell_sort(string_t s);


bst_node_t bst_search_get(bst_node_t root, string_t key);
int _bst_create_node(bst_node_t *node_ptr, string_t key, void *data);
int bst_insert(bst_node_t *root_ptr, string_t key, void *data);
void bst_postorder(bst_node_t root, void (*do_work)(bst_node_t));

#endif
