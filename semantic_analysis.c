#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"
#include <stdio.h>

bool print_bullshit;

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

void sem_set_active_class(string_t id) {
    active_class = st_getclass(id);
}

void sem_set_active_fn(string_t id) {
    active_function = st_getmemb(active_class, id);
}

int sem_prec_reduction() {
    return 42;
}

static void _print_decoded_id(void *symbol) {
    if(!print_bullshit) return;
    if(symbol == NULL) {
        printf("Nothing decoded.. \n");
        return;
    }
    local_var_t elem = (local_var_t) symbol;
    if(elem->sc != literal) {
        printf("Decoded id (%d): %s (%p)\n", elem->sc, elem->id->data, symbol);
    }
}

static void _print_demand() {
    if(!print_bullshit) return;
    if(sem_id_decoded.class_id != NULL) {
        printf("Demanded: %s.%s\n", sem_id_decoded.class_id->data, sem_id_decoded.memb_id->data);
    } else  {
        printf("Demanded: %s\n", sem_id_decoded.memb_id->data);
    }
}

void sem_search() {
    print_bullshit = false;
    _print_demand();
    void *symbol = NULL;
    if (sem_id_decoded.class_id == NULL) {
        symbol = st_get_loc_var(active_function, sem_id_decoded.memb_id);
        if(!symbol)
            symbol = st_getmemb(active_class, sem_id_decoded.memb_id);
    } else {
        class_t class = st_getclass(sem_id_decoded.class_id);
        if (class) {
            symbol = st_getmemb(class, sem_id_decoded.memb_id);
        }
    }
    sem_id_decoded.ptr = symbol;
    setIsFunFlag(symbol);
    _print_decoded_id(symbol);
}

void setIsFunFlag(void *symbol) {
    if(symbol != NULL) {
        class_memb_t memb = (class_memb_t)symbol;
        if(memb->sc == global)
            if(memb->type == func)
                sem_id_decoded.isFun = true;
            else
                sem_id_decoded.isFun = false;
        else
            sem_id_decoded.isFun = false;
    }
}