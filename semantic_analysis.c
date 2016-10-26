#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"
#include <stdio.h>

int new_class(string_t id) {
    class_t c;
    int err = insert_class(id, &c);
    if (err == 0) {
        active_class = c;
        printf("Class: %s added.\n", id->data);
        return 0;
    } else if(err == -1) { // TODO
        fprintf(stderr, "ERR: Can't redefine class.\n");
    } else if(err == -2) { // TODO
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
}