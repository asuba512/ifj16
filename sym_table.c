#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "infinite_string.h"
#include "ial.h"
#include "sym_table.h"
#include "ilist.h"
#include "gc.h"

void init_class_table() {
	classes = &ctable; // that's all
	glob_helper_vars.head = glob_helper_vars.tail = NULL; // initializing literal array as well
	glob_instr_list.head = glob_instr_list.tail = NULL; // initializing global instruction tape as well
}

/** Tries to add a new class to Level 1 of ST (table of classes).
	Returns 0 on success, otherwise corresponding error code
	is returned (either 3-redefinition or 99-internal error) */
int insert_class(string_t id, class_t *target) {
	class_t new_class;
	if ((new_class = malloc(sizeof(struct class))) == NULL)
		return 99;
	// initialization
	new_class->root = NULL;
	int err;
	if((err = bst_insert(&(classes->root), id, (void *)new_class)) == 0) {
		*target = new_class;
		return 0; // OK
	}
	free(new_class);
	return err;
}

/** Tries to add class member (var or func) to Level 2 of ST
	(table of class members). Returns 0 on success, otherwise
	corresponding error code is returned (either 3-redefinition
	or 99-internal error) */
int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt) {
	class_memb_t m;
	if ((m = malloc(sizeof(struct class_memb))) == NULL)
		return 99;
	// initialization
	m->type = type; // variable or func?
	m->op.dtype = dt; // datatype
	m->arg_count = m->var_count = m->_max_arg_count = 0;
	m->arg_list = NULL; // will be allocated when needed
	m->local_sym_table_root = NULL;
	m->initialized = false; // variable is not initialized by default
	m->second_pass = false; // is marked "true" when second pass reaches this symbol
	m->helper_vars = NULL;	// has no helper variables by default
	m->op.sc = global; // all symbols in Level 2 are global
	m->instr_list = m->instr_list_end = NULL; // no instructions by default
	int err;
	if((err = bst_insert(&(c->root), id, (void *)m)) == 0) {
		*target = m;
		return 0; // OK
	}
	free(m); // ST doesn't use gabage collector
	*target = NULL; // causes segfault (intentionally) when program continues ilegally
	return err;
}

/** Adds additional space to table of POINTERS (!!!) to function arguments
	(which are located in Level 3 ST mixed with other local vars). */
static int _add_fn_arg_space(class_memb_t fn) {
	local_var_t *new_space = realloc(fn->arg_list, (fn->_max_arg_count + ARG_CHUNK) * sizeof(local_var_t));
	if(new_space == NULL)
		return 99; // internal error
	fn->arg_list = new_space;
	fn->_max_arg_count += ARG_CHUNK;
	return 0;
}

/** Adds new argument symbol to Level 3 ST of function 'fn' with datatype 'dt'
	and identifier 'id' and stores copy of pointer in array of POINTERS to function args.
*/
int st_add_fn_arg(class_memb_t fn, datatype dt, string_t id) {
	local_var_t lv;
	if ((lv = malloc(sizeof(struct local_var))) == NULL)
		return 99;
	// this thing allows us to search for function arguments by their indexes
	if(fn->arg_count == fn->_max_arg_count) {
		if(_add_fn_arg_space(fn) == 99) {
			free(lv);
			return 99;
		}
	}
	(fn->arg_list)[fn->arg_count] = lv;
	(fn->var_count)++;
	// initialization
	lv->op.dtype = dt;
	lv->index = (fn->arg_count)++;
	lv->op.sc = local; // scope of argument is local
	// lv->next is unused because those variables are stored in BST
	int err;
	if((err = bst_insert(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) {
		(fn->arg_count)--; // doesn't matter anyways, if there's an error
		(fn->var_count)--;
	}
	free(lv);
	return err;
}


/** Adds new local var symbol to Level 3 ST of function 'fn' with datatype 'dt'
	and identifier 'id' and DOES NOT store any copy of pointer anywhere else. */
int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id) {
	local_var_t lv;
	if ((lv = malloc(sizeof(struct local_var))) == NULL)
		return 99;
	lv->op.dtype = dt;
	lv->index = (fn->var_count)++;
	lv->op.sc = local;
	// lv->next is unused because those variables are stored in BST
	int err;
	if((err = bst_insert(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) (fn->var_count)--; // doesn't matter anyways, if there's an error
	free(lv);
	return err;
}

/** Searches for class in Level1 ST. Returns pointer to class structure
	upon success, otherwise returns NUL. */
class_t st_getclass(string_t id) {
	bst_node_t result = bst_search_get(classes->root, id);
	return result ? (class_t)(result->data) : NULL;
}

/** Searches for class member in Level2 ST of class 'c'. Returns pointer
	to class member structure upon success, otherwise returns NUL. */
class_memb_t st_getmemb(class_t c, string_t id) {
	bst_node_t result = bst_search_get(c->root, id);
	return result ? (class_memb_t)(result->data) : NULL;
}

/** Searches for local var/argument in Level3 ST of function 'm'. Returns pointer
	to variable structure upon success, otherwise returns NUL. */
local_var_t st_get_loc_var(class_memb_t m, string_t id) {
	bst_node_t result = bst_search_get(m->local_sym_table_root, id);
	return result ? (local_var_t)(result->data) : NULL;
}

/** Adds new entry to the list of global helper variables (and literals).
	Retrieves necessary info from given token 't'. Second parameter specifies,
	whether the entry has to be marked as initialized by default (in case of literal). */
glob_helper_var_t add_global_helper_var(struct token t, bool initialized) {
	glob_helper_var_t new_node = malloc(sizeof(struct global_helper_var));
    if(new_node == NULL)
        return NULL;
	// working with queue (the way these are stored is NOT IMPORTANT during analysis or interpretation)
    new_node->next = NULL;
    if(glob_helper_vars.head == NULL)
        glob_helper_vars.head = new_node;
    else
        glob_helper_vars.tail->next = new_node;
    glob_helper_vars.tail = new_node;
	switch(t.type) {
		case token_double:
			(new_node->val).d_val = t.attr.d;
			(new_node->op).dtype = dt_double;
			break;
		case token_int:
			(new_node->val).i_val = t.attr.i;
			(new_node->op).dtype = dt_int;
			break;
		case token_string:
			(new_node->val).s_val = t.attr.s;
			(new_node->op).dtype = dt_String;
			break;
		case token_boolean:
			(new_node->val).b_val = t.attr.b;
			(new_node->op).dtype = dt_boolean;
			break;
		default:
			break;
	}
	(new_node->op).sc = helper; // scope is 'helper' by default and differs from 'global' just by size
	new_node->initialized = initialized;
	return new_node;
}

/** Appends new instruction to the instruction list of function 'fn'
	Returns 0 on success, otherwise returns 99 (internal error) */
int st_add_fn_instr(class_memb_t fn, struct instr i) {
	instr_t new_instr = malloc(sizeof(struct instr));
    if(new_instr == NULL)
        return 99;
	*new_instr = i;
	// work with queue (the way it's stored IS IMPORTANT during interpretation -> correct order is needed)
	new_instr->next = NULL;
    if(fn->instr_list == NULL)
        fn->instr_list = new_instr;
    else
		fn->instr_list_end->next = new_instr;
    fn->instr_list_end = new_instr;
    return 0;
}

/** Adds new helper/temporary variable to list of temp variables
	of function 'fn' with datatype 'dt'. This kind of variable
	does not have any identifier, therefore is not stored in BST.
	Returns pointer to newly creaded temp var on success, otherwise NULL. */
local_var_t st_fn_add_tmpvar(class_memb_t fn, datatype dt) {
	local_var_t tmp; // same structure as regular local variable
	if ((tmp = malloc(sizeof(struct local_var))) == NULL)
		return NULL;
	tmp->op.dtype = dt;
	tmp->index = (fn->var_count)++; // this kind of var also needs to have its unique place on the stackframe
	tmp->op.sc = local;
	// working with stack-like linked list (the way those are stored is NOT IMPORTANT during analysis or interpretation)
	tmp->next = fn->helper_vars;
	// tmp->next is actually only used in temporary vars
	fn->helper_vars = tmp;
	return tmp;
}

/** Appends new instruction to the global instruction tape (as a result
	of static var initialization or adding instructions to call Main.run)
	Returns 0 on success, otherwise 99-internal err. */
int st_add_glob_instr(struct instr i) {
	instr_t new_instr = gc_malloc(sizeof(struct instr));
    if(new_instr == NULL)
        return 99;
	*new_instr = i;
	// working with queue (the way instructions are stored IS IMPORTANT for interpretation -> we need correct order)
	new_instr->next = NULL;
    if(glob_instr_list.head == NULL)
        glob_instr_list.head = new_instr;
    else
		glob_instr_list.tail->next = new_instr;
    glob_instr_list.tail = new_instr;
    return 0;
}

/** Function to erase all the data from symbols table, free the memory
	and prepare for exit. PostOrder-s are used to erase BSTs. */
void st_destroy_all() {
	// crawls entire Level1 ST and calls destroy_class() on every node.
	bst_postorder(classes->root, destroy_class); 
	// dispose linked list
	glob_helper_var_t previous = glob_helper_vars.head;
	if(!previous) return; // empty list
    for(glob_helper_var_t item = previous->next; item != NULL; item = item->next) {
        free(previous);
        previous = item;
    }
    free(previous);
}

/** Function to erase all the data associated with class stored in BST node 'c'. */
void destroy_class(bst_node_t c) {
	// crawls entire Level2 ST and calls destroy_class_memb() on every node (vars and functions)
	bst_postorder(((class_t)(c->data))->root, destroy_class_memb);
	// free the actual class entry and its node in BST
	free(c->data);
	free(c);
}

/** Function to erase all the data associated with class member stored in BST node 'm' */
void destroy_class_memb(bst_node_t m) {
	// no need to check before freeing, because arg_list is always NULL (free does nothing) or holds some allocated space
	free(((class_memb_t)(m->data))->arg_list); 
	// crawls entire Level3 ST and calls destroy_loc_var() on every node (local vars and arguments)
	bst_postorder(((class_memb_t)(m->data))->local_sym_table_root, destroy_loc_var);
	// if function has nonempty instruction list, we need to dispose the linked list
	if (((class_memb_t)(m->data))->instr_list != NULL) {
		instr_t previous = ((class_memb_t)(m->data))->instr_list;
		for(instr_t item = previous->next; item != NULL; item = item->next) {
			free(previous);
			previous = item;
		}
		free(previous);
	}
	// if function has nonempty helper vars list, we need to dispose the linked list
	if(((class_memb_t)(m->data))->helper_vars != NULL) {
		local_var_t previous = ((class_memb_t)(m->data))->helper_vars;
		for(local_var_t item = previous->next; item != NULL; item = item->next) {
			free(previous);
			previous = item;
		}
		free(previous);
	}
	// free the actual class member and its node
	free(m->data);
	free(m);
}

/** Function to erase single local var structure in Level3 BST (local variables and arguments) */
void destroy_loc_var(bst_node_t v) {
	// just free the local var structure and its node
	free(v->data);
	free(v);
}
