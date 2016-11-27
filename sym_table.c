#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "infinite_string.h"
#include "ial.h"
#include "sym_table.h"
#include "ilist.h"
#include "gc.h"

void init_class_table() {
	classes = &ctable; // yep that's all
	glob_helper_vars.length = 0; // initializing literal array as well
	glob_helper_vars.max_length = 0;
}

int insert_class(string_t id, class_t *target) {
	class_t new_class;
	if ((new_class = malloc(sizeof(struct class))) == NULL)
		return 99;
	// initialization
	new_class->root = NULL;
	//new_class->id = id;
	int err;
	if((err = bst_insert_or_err(&(classes->root), id, (void *)new_class)) == 0) {
		*target = new_class;
		return 0; // OK
	}
	free(new_class);
	return err;
} // OK

int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt) {
	class_memb_t m;
	if ((m = malloc(sizeof(struct class_memb))) == NULL)
		return 99;
	// initialization
	m->type = type; // variable or func?
	m->op.dtype = dt; // datatype
	m->arg_count = m->var_count = m->_max_arg_count = 0;
	m->arg_list = NULL;
	m->local_sym_table_root = NULL;
	m->initialized = false;
	//m->id = id; // UNUSEFUL
	m->op.sc = global;
	m->instr_list = m->instr_list_end = NULL;
	int err;
	if((err = bst_insert_or_err(&(c->root), id, (void *)m)) == 0) {
		*target = m;
		return 0; // OK
	}
	free(m);
	*target = NULL; // causes segfault (intentionally) when program continues ilegally
	return err;
} // OK

static int _add_fn_arg_space(class_memb_t fn) {
	local_var_t *new_space = realloc(fn->arg_list, (fn->_max_arg_count + 5) * sizeof(local_var_t));
	if(new_space == NULL)
		return 99;
	fn->arg_list = new_space;
	fn->_max_arg_count += 5;
	return 0;
} // OK

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
	//lv->id = id;
	lv->op.sc = local;
	int err;
	if((err = bst_insert_or_err(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) {
		(fn->arg_count)--; // doesn't matter anyways, if there's an error
		(fn->var_count)--;
	}
	free(lv);
	return err;
} // OK

int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id) {
	local_var_t lv;
	if ((lv = malloc(sizeof(struct local_var))) == NULL)
		return 99;
	lv->op.dtype = dt;
	lv->index = (fn->var_count)++;
	//lv->id = id;
	lv->op.sc = local;
	int err;
	if((err = bst_insert_or_err(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) (fn->var_count)--; // doesn't matter anyways, if there's an error
	free(lv);
	return err;
} // OK

class_t st_getclass(string_t id) {
	bst_node_t result = bst_search_get(classes->root, id);
	return result ? (class_t)(result->data) : NULL;
} // OK

class_memb_t st_getmemb(class_t c, string_t id) {
	bst_node_t result = bst_search_get(c->root, id);
	return result ? (class_memb_t)(result->data) : NULL;
} // OK

local_var_t st_get_loc_var(class_memb_t m, string_t id) {
	bst_node_t result = bst_search_get(m->local_sym_table_root, id);
	return result ? (local_var_t)(result->data) : NULL;
} // OK

static int _add_global_helper_var_space() {
	glob_helper_var_t new_space = gc_realloc(glob_helper_vars.arr, (glob_helper_vars.length + 100) * sizeof(struct global_helper_var));
	if(new_space == NULL)
		return 99;
	glob_helper_vars.arr = new_space;
	glob_helper_vars.max_length += 100;
	return 0;
} // OK

glob_helper_var_t add_global_helper_var(struct token t, bool initialized) {
	if(glob_helper_vars.length == glob_helper_vars.max_length) 
		if(_add_global_helper_var_space() != 0)
			return NULL;
	switch(t.type) {
		case token_double:
			glob_helper_vars.arr[glob_helper_vars.length].val.d_val = t.attr.d;
			glob_helper_vars.arr[glob_helper_vars.length].op.dtype = dt_double;
			break;
		case token_int:
			glob_helper_vars.arr[glob_helper_vars.length].val.i_val = t.attr.i;
			glob_helper_vars.arr[glob_helper_vars.length].op.dtype = dt_int;
			break;
		case token_string:
			glob_helper_vars.arr[glob_helper_vars.length].val.s_val = t.attr.s;
			glob_helper_vars.arr[glob_helper_vars.length].op.dtype = dt_String;
			break;
		case token_boolean:
			glob_helper_vars.arr[glob_helper_vars.length].val.b_val = t.attr.b;
			glob_helper_vars.arr[glob_helper_vars.length].op.dtype = dt_boolean;
			break;
		default:
			break;
	}
	glob_helper_vars.arr[glob_helper_vars.length].op.sc = helper; // just interpret things
	glob_helper_vars.arr[glob_helper_vars.length].initialized = initialized;
	glob_helper_vars.length++;
	return &(glob_helper_vars.arr[glob_helper_vars.length-1]);
} // OK

int st_add_fn_instr(class_memb_t fn, struct instr i) {
	instr_t new_instr = malloc(sizeof(struct instr));
    if(new_instr == NULL)
        return 99;
	*new_instr = i;
	new_instr->next = NULL;
    if(fn->instr_list == NULL)
        fn->instr_list = new_instr;
    else
		fn->instr_list_end->next = new_instr;
    fn->instr_list_end = new_instr;
    return 0;
} // OK

local_var_t st_fn_add_tmpvar(class_memb_t fn, datatype dt, string_t id) {
	local_var_t tmp;
	if ((tmp = malloc(sizeof(struct local_var))) == NULL)
		return NULL;
	tmp->op.dtype = dt;
	tmp->index = (fn->var_count)++;
	//tmp->id = id;
	tmp->op.sc = local;
	int err;
	if((err = bst_insert_or_err(&(fn->local_sym_table_root), id, (void *)tmp)) == 0) {
		return tmp; // OK
	}
	free(tmp);
	return NULL;
} // OK

void st_init_glob_instr_list() {
	glob_instr_list.head = glob_instr_list.tail = NULL;
} // OK

int st_add_glob_instr(struct instr i) {
	instr_t new_instr = gc_malloc(sizeof(struct instr));
    if(new_instr == NULL)
        return 99;
	*new_instr = i;
	new_instr->next = NULL;
    if(glob_instr_list.head == NULL)
        glob_instr_list.head = new_instr;
    else
		glob_instr_list.tail->next = new_instr;
    glob_instr_list.tail = new_instr;
    return 0;
} // OK

void st_destroy_all() {
	bst_postorder(classes->root, destroy_class);
}

void destroy_class(bst_node_t c) {
	bst_postorder(((class_t)(c->data))->root, destroy_class_memb);
	free(c->data);
	free(c);
}

void destroy_class_memb(bst_node_t m) {
	free(((class_memb_t)(m->data))->arg_list);
	bst_postorder(((class_memb_t)(m->data))->local_sym_table_root, destroy_loc_var);
	if (((class_memb_t)(m->data))->instr_list != NULL) {
		instr_t previous = ((class_memb_t)(m->data))->instr_list;
		// zakadym sa posunieme o prvok dopredu a zmazene predchadzajuci
		for(instr_t item = previous->next; item != NULL; item = item->next) {
			free(previous);
			previous = item;
		}
		free(previous);
		((class_memb_t)(m->data))->instr_list = NULL;
	}
	free(m->data);
	free(m);
}

void destroy_loc_var(bst_node_t v) {
	free(v->data);
	free(v);
}