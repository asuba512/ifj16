
#include "infinite_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ial.h"

static bst_node_t _bst_helper_ptr;

// Algorithm from "Opora-IAL-2014-verze-15-A.pdf" (page 174)
void BMA_compute_jumps(string_t substr, unsigned *char_jump) {
    for(int i=0; i<255; i++){ //the interval is from algorithm
        char_jump[i] =substr->length;
    } 
    for(int k=0; k<substr->length; k++){
        char_jump[(int)substr->data[k]] = substr->length-k-1;
    }
    return;
}

void BMA_compute_match_jumps(string_t substr,unsigned *match_jump){
    int k,q,qq;
    int m=substr->length;
    unsigned backup[m+1];

    for (int k=0;k<m;k++){
        match_jump[k]=2*m-k;
    }
    k=m-1;
    q=m;

    while(k>=0) {
        backup[k]=q;
        while((q<m) && (substr->data[k] != substr->data[q])) {
            if ((int)match_jump[q] > m-k) {
                match_jump[q] = m-k;
            }
            q=backup[q];
        }
        k=k-1;
        q=q-1;
    }

    for(int k=0;k<q;k++) {
        if ((int)match_jump[k] > m+q-k) {
            match_jump[k]=m+q-k;
        }
    }
	qq=backup[q];
    while(q<m) {
        while(q<=qq) {
            if((int)match_jump[q]>qq-q+m) {
                match_jump[q]=qq-q+m;
            }
        q=q+1;
        }
    qq=backup[qq];
    }
}



// Algorithm from "Opora-IAL-2014-verze-15-A.pdf" (page 174)
int BMA(string_t str, string_t substr, unsigned *char_jump, unsigned *match_jump) {
    int index;
    int j = substr->length-1;
    int k = substr->length-1;
    while( j<str->length && k>=0 ){
        if (str->data[j] == substr->data[k]) {
            j = j-1;
            k = k-1;
        }//from here
        else {
			if(char_jump[(int)str->data[j]] > match_jump[k]) {
				j = j+char_jump[(int)str->data[j]];
			}
			else {
				j = j+match_jump[k];
			}
		    k = substr->length-1;
        }
    }
    if (k == -1) {
        index = j+1;
    }
    else {
        index = str->length; //from algorithm,maybe will use -1?
    }       
    return index;
}

unsigned BMA_index(string_t str,string_t substr) {
	int index;
	unsigned compute_jumps_array[255];//in compute jumps for from 0 to 254(255 items)
    unsigned match_jumps_array[substr->length+1];//in compute match jumps indexed with q(q=substr->length+1)
    BMA_compute_jumps(substr,compute_jumps_array);
    BMA_compute_match_jumps(substr,match_jumps_array);
    index=BMA(substr,str,compute_jumps_array,match_jumps_array);
    return index;
}

// Algorithm from "Opora-IAL-2014-verze-15-A.pdf" (page 156)
void shell_sort(string_t s) {
    char tmp;
    int i,j;
    int step = s->length / 2;
    while (step > 0) {
        for(i=step; i<=((s->length)-1); i++) {
            j=i-step;
            while ((j>=0) && (s->data[j]>s->data[j+step])) {
                tmp = s->data[j];               // ====
                s->data[j] = s->data[j+step];   // SWAP
                s->data[j+step] = tmp;          // ====
                j -= step;
            }
        }
        step = step/2;
    }
    return;
}

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

int bst_insert(bst_node_t *root_ptr, string_t key, void *data) {
    if(*root_ptr == NULL) {
        return _bst_create_node(root_ptr, key, data);
    } else {
        if(str_compare((*root_ptr)->key, key) > 0) {
            return bst_insert(&((*root_ptr)->left_p), key, data);
        } else if(str_compare((*root_ptr)->key, key) < 0) {
            return bst_insert(&((*root_ptr)->right_p), key, data);
        } else {
            (*root_ptr)->data = data;
            return 0;
        }
    }
}

// almost copy-paste of function above
int bst_insert_or_err(bst_node_t *root_ptr, string_t key, void *data) {
    if(*root_ptr == NULL) {
        return _bst_create_node(root_ptr, key, data);
    } else {
        int cmp = str_compare((*root_ptr)->key, key);
        if(cmp > 0) {
            return bst_insert_or_err(&((*root_ptr)->left_p), key, data);
        } else if(cmp < 0) {
            return bst_insert_or_err(&((*root_ptr)->right_p), key, data);
        } else {
            // normally would update data, but why the hell would I want to cause a memory leak? why??
            return BST_NODE_ALREADY_EXISTS;
        }
    }
}

int _bst_create_node(bst_node_t *node_ptr, string_t key, void *data) {
    *node_ptr = malloc(sizeof(struct bst_node));
    if(*node_ptr == NULL) return BST_MEM_ERR;
    (*node_ptr)->left_p = (*node_ptr)->right_p = NULL;
    (*node_ptr)->key = key;
    (*node_ptr)->data = data;
    return 0;
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

void bst_inorder(bst_node_t root, void (*do_work)(void *)) {
    if(root != NULL) {
        bst_inorder(root->left_p, do_work);
        do_work(root->data);
        bst_inorder(root->right_p, do_work);
        
    }
}
