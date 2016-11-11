#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "infinite_string.h"
#include "ial.h"
#include "sym_table.h"
#include "ilist.h"

void init_class_table() {
	classes = &ctable; // yep that's all
	literals.length = 0; // initializing literal array as well
	literals.max_length = 0;
}

int insert_class(string_t id, class_t *target) {
	class_t new_class;
	if ((new_class = malloc(sizeof(struct class))) == NULL)
		return 99;
	// initialization
	new_class->root = NULL;
	new_class->id = id;
	int err;
	if((err = bst_insert_or_err(&(classes->root), id, (void *)new_class)) == 0) {
		*target = new_class;
		return 0; // OK
	}
	free(new_class);
	return err;
}

int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt) {
	class_memb_t m;
	if ((m = malloc(sizeof(struct class_memb))) == NULL)
		return 99;
	// initialization
	m->type = type;
	m->dtype = dt;
	m->arg_count = m->var_count = m->_max_arg_count = 0;
	m->arg_list = NULL;
	m->local_sym_table_root = NULL;
	m->initialized = false;
	m->id = id;
	m->sc = global;
	m->instr_list = m->instr_list_end = NULL;
	int err;
	if((err = bst_insert_or_err(&(c->root), id, (void *)m)) == 0) {
		*target = m;
		return 0; // OK
	}
	free(m);
	return err;
}

static int _add_fn_arg_space(class_memb_t fn) {
	local_var_t *new_space = realloc(fn->arg_list, (fn->_max_arg_count + 5) * sizeof(local_var_t));
	if(new_space == NULL)
		return 99;
	fn->arg_list = new_space;
	fn->_max_arg_count += 5;
	return 0;
}

int st_add_fn_arg(class_memb_t fn, datatype dt, string_t id) {
	local_var_t lv;
	if ((lv = malloc(sizeof(struct local_var))) == NULL)
		return 99;
	// this shat allows us to search for function arguments by their indexes
	if(fn->arg_count == fn->_max_arg_count) {
		if(_add_fn_arg_space(fn) == 99) {
			free(lv);
			return 99;
		}
	}
	(fn->arg_list)[fn->arg_count] = lv;
	(fn->var_count)++;
	// initialization
	lv->dtype = dt;
	lv->index = (fn->arg_count)++;
	lv->id = id;
	lv->sc = local;
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
}

int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id) {
	local_var_t lv;
	if ((lv = malloc(sizeof(struct local_var))) == NULL)
		return 99;
	lv->dtype = dt;
	lv->index = (fn->var_count)++;
	lv->id = id;
	lv->sc = local;
	int err;
	if((err = bst_insert_or_err(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) (fn->var_count)--; // doesn't matter anyways, if there's an error
	free(lv);
	return err;
}

class_t st_getclass(string_t id) {
	bst_node_t result = bst_search_get(classes->root, id);
	return result ? (class_t)(result->data) : NULL;
}


class_memb_t st_getmemb(class_t c, string_t id) {
	bst_node_t result = bst_search_get(c->root, id);
	return result ? (class_memb_t)(result->data) : NULL;
}

local_var_t st_get_loc_var(class_memb_t m, string_t id) {
	bst_node_t result = bst_search_get(m->local_sym_table_root, id);
	return result ? (local_var_t)(result->data) : NULL;
}

static int _add_literal_space() {
	literal_t new_space = realloc(literals.arr, (literals.length + 100) * sizeof(struct literal));
	if(new_space == NULL)
		return 99;
	literals.arr = new_space;
	literals.max_length += 100;
	return 0;
}

literal_t add_literal(struct token t) {
	if(literals.length == literals.max_length) 
		if(_add_literal_space() != 0) {
			return NULL;
		}
	switch(t.type) {
		case token_double:
			literals.arr[literals.length].d_val = t.attr.d;
			literals.arr[literals.length].dtype = dt_double;
			break;
		case token_int:
			literals.arr[literals.length].i_val = t.attr.i;
			literals.arr[literals.length].dtype = dt_int;
			break;
		case token_string:
			literals.arr[literals.length].s_val = t.attr.s;
			literals.arr[literals.length].dtype = dt_String;
			break;
		case token_boolean:
			literals.arr[literals.length].b_val = t.attr.b;
			literals.arr[literals.length].dtype = dt_boolean;
			break;
		default:
			break;
	}
	literals.arr[literals.length].sc = literal; // just interpret things
	literals.length++;
	return &(literals.arr[literals.length-1]);
}

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
}