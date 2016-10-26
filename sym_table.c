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
		return -1;
	new_class->root = NULL;
	int err;
	if((err = bst_insert_or_err(&(classes->root), id, (void *)new_class)) == 0) {
		*target = new_class;
		return 0; // OK
	}
	free(new_class);
	return -1; // replace with actual error codes, ie. semantic error, internal error, use err variable
}



// later
// int add_function(class_t c, string_t key, )