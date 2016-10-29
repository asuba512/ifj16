#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "infinite_string.h"
#include "ial.h"
#include "sym_table.h"

void init_class_table() {
	classes = &ctable; // yep that's all
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
	m->l_g = global;
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
	lv->l_g = local;
	int err;
	if((err = bst_insert_or_err(&(fn->local_sym_table_root), id, (void *)lv)) == 0) {
		return 0; // OK
	}
	if(err == 3) (fn->arg_count)--; // doesn't matter anyways, if there's an error
	free(lv);
	return err;
}