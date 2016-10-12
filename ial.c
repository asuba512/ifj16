
#include "infinite_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ial.h"

static bst_node_t _bst_helper_ptr;

bool bst_search(bst_node_t root, string_t key) {
	if(root != NULL) {
		if(str_compare(root->key, key) == 0) {
			return true;
		} else {
			if(str_compare(root->key, key) > 0) {
				return bst_search(root->left_p, key);
			} else {
				return bst_search(root->right_p, key);
			}
		}
	} else {
		return false;
	}
}

bst_node_t bst_search_get(bst_node_t root, string_t key) {
	if(root == NULL) {
		return NULL;
	} else {
		if(str_compare(root->key, key) != 0) {
			if(str_compare(root->key, key) > 0) {
				return bst_search_get(root->left_p, key);
			} else {
				return bst_search_get(root->right_p, key);
			}
		} else {
			return root;
		}
	}
}

void bst_insert(bst_node_t *root_ptr, string_t key, int data) {
	if(*root_ptr == NULL) {
		_bst_create_node(root_ptr, key, data);
	} else {
		if(str_compare((*root_ptr)->key, key) > 0) {
			bst_insert(&((*root_ptr)->left_p), key, data);
		} else if(str_compare((*root_ptr)->key, key) < 0) {
			bst_insert(&((*root_ptr)->right_p), key, data);
		} else {
			(*root_ptr)->data = data;
		}
	}
}

void _bst_create_node(bst_node_t *node_ptr, string_t key, int data) {
	*node_ptr = malloc(sizeof(struct bst_node));
	(*node_ptr)->left_p = (*node_ptr)->right_p = NULL;
	(*node_ptr)->key = key;
	(*node_ptr)->data = data;
}

void bst_delete(bst_node_t *root, string_t key) {
	if(*root != NULL) {
		if(str_compare((*root)->key, key) > 0) {
			bst_delete(&((*root)->left_p), key);
		} else if(str_compare((*root)->key, key) < 0) {
			bst_delete(&((*root)->right_p), key);
		} else {
			_bst_helper_ptr = *root;
			if(_bst_helper_ptr->right_p == NULL) {
				*root = _bst_helper_ptr->left_p;
			} else if(_bst_helper_ptr->left_p == NULL) {
				*root = _bst_helper_ptr->right_p;
			} else {
				_bst_del(&(_bst_helper_ptr->left_p));
			}
			str_destroy(_bst_helper_ptr->key);
			free(_bst_helper_ptr);
			_bst_helper_ptr = NULL;
		}
	}
}

void _bst_del(bst_node_t *node) {
	if((*node)->right_p != NULL) {
		_bst_del(&((*node)->right_p));
	} else {
		_bst_helper_ptr->data = (*node)->data;
		_bst_helper_ptr->key = (*node)->key;
		_bst_helper_ptr = *node;
		*node = (*node)->left_p;
	}
}
