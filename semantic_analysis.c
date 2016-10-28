#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"
#include <stdio.h>

int sem_new_class(string_t id) {
    class_t c;
    int err = insert_class(id, &c);
    if (err == 0) {
        active_class = c;
        //printf("Class: %s added.\n", id->data);
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Can't redefine class \"%s\".\n", id->data);
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
}

int sem_add_member_active_class(var_func member_type) {
    class_memb_t new_memb;
    int err = st_insert_class_memb(active_class, &new_memb, sem_tmp_data.id, member_type, sem_tmp_data.dt);
    if (err == 0) {
        if(member_type == func) {
            active_function = new_memb;
        }
        //printf("Member: %s added - %s, %d.\n", sem_tmp_data.id->data, new_memb->type == var ? "var" : "func", new_memb->dtype);
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Class member redefinition.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
}

int sem_add_arg_active_fn() {
    int err = st_add_fn_arg(active_function, sem_tmp_data.dt, sem_tmp_data.id);
    if (err == 0) {
        //printf("Function argument: %s added, %d.\n", sem_tmp_data.id->data, sem_tmp_data.dt);
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Function argument with same identifier already exists.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
}

int sem_prec_reduction() {
    return 42;
}