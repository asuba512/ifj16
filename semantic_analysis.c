#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"

#include <stdio.h>

#define isNum(x) (x->dtype == dt_double || x->dtype == dt_int)

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
    } else
        sem_id_decoded.isFun = false;
}

int sem_new_loc_var(datatype dt, string_t id) {
    int err = st_add_fn_locvar(active_function, dt, id);
    if (err == 0) {
        //printf("Function local var: %s added, %d.\n", id->data, dt);
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Function local variable with same identifier already exists.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
}

local_var_t sem_new_tmp_var(datatype dt) {
    static int id = 0;
    char name[8];
    sprintf(name, "^%d", id);
    string_t str = str_init(name);
    if (str == NULL) {
        fprintf(stderr, "ERR: Internal error.\n");
        return NULL;
    }
    local_var_t tmpvar = st_fn_add_tmpvar(active_function, dt, str);
    if (tmpvar == NULL) {
        fprintf(stderr, "ERR: Internal error.\n");
        str_destroy(str);
        return NULL;
    }
    id++;
    return tmpvar;
}

int sem_generate_arithm(instr_type_t type, op_t src1, op_t src2, op_t *dst) {
    op_t new_var, new_dst;
    struct instr i, conv; // i - main instruction, conv - helper conversion instruction
    // just default values
    i.type = type;
    i.src1 = src1;
    i.src2 = src2;
    *dst = NULL;
    // dst will be set at the end because we don't know its datatype yet

    if(src1->dtype == dt_String || (src2 && src2->dtype == dt_String)) {
        if(type == add) {
            i.type = conc; // concatenation of strings and addicion are different instructions
            new_dst = (op_t)sem_new_tmp_var(dt_String); // result of concatenation
            // conversion and further modifications are needed, when one of operands is not a string  
            if(src1->dtype != dt_String || src2->dtype != dt_String) {   
                new_var = (op_t)sem_new_tmp_var(dt_String); // converted operand     
                conv.src2 = NULL; 
                if(src1->dtype != dt_String) {
                    switch(src1->dtype) {
                        case dt_double: conv.type = dbl_to_str; break;
                        case dt_int: conv.type = int_to_str; break;
                        case dt_boolean: conv.type = bool_to_str; break;
                        default: break;
                    }
                    conv.src1 = src1;
                    i.src1 = new_var;
                } else {
                    switch(src2->dtype) {
                        case dt_double: conv.type = dbl_to_str; break;
                        case dt_int: conv.type = int_to_str; break;
                        case dt_boolean: conv.type = bool_to_str; break;
                        default: break;
                    }
                    conv.src1 = src2;
                    i.src2 = new_var;
                }  
                conv.dst = new_var;
                if(st_add_fn_instr(active_function, conv) != 0) {
                    fprintf(stderr, "ERR: Internal error.\n");
                    return 99;
                }
            }
            
        } else {
            fprintf(stderr, "ERR: Strings can only be concatenated with '+'.\n");
            return 4;
        }
    } else {
        if(type == add || type == sub || type == imul || type == idiv) {
            if(isNum(src1) && isNum(src2)) {
                if(src1->dtype == dt_int && src2->dtype == dt_int) {
                    new_dst = (op_t)sem_new_tmp_var(dt_int); // TODO internal err
                } else {
                    new_dst = (op_t)sem_new_tmp_var(dt_double); // TODO internal err
                }
            } else {
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == eql || type == neq) { // either numbers or booleans can be tested for equivalence
            new_dst = (op_t)sem_new_tmp_var(dt_boolean); // TODO internal err
            if(isNum(src1) && isNum(src2)) {
                if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                    new_var = (op_t)sem_new_tmp_var(dt_double); // converted operand
                    conv.type = int_to_dbl;
                    conv.src2 = NULL;
                    conv.dst = new_var; 
                    if(src1->dtype == dt_double && src2->dtype == dt_int) {
                        conv.src1 = src2;
                        i.src2 = new_var;
                    } else /* (src1->dtype == dt_int && src2->dtype == dt_double) */ {
                        conv.src1 = src1;
                        i.src1 = new_var;
                    }
                    st_add_fn_instr(active_function, conv);
                }
            } else if(src1->dtype == dt_boolean && src2->dtype == dt_boolean) {
                // OK
            } else { // combination of number and boolean is prohibited
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == leq || type == geq || type == gre || type == less) { 
            new_dst = (op_t)sem_new_tmp_var(dt_boolean); // TODO internal err
            if(isNum(src1) && isNum(src2)) {
                if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                    new_var = (op_t)sem_new_tmp_var(dt_double); // converted operand
                    conv.type = int_to_dbl;
                    conv.src2 = NULL;
                    conv.dst = new_var; 
                    if(src1->dtype == dt_double && src2->dtype == dt_int) {
                        conv.src1 = src2;
                        i.src2 = new_var;
                    } else /* (src1->dtype == dt_int && src2->dtype == dt_double) */ {
                        conv.src1 = src1;
                        i.src1 = new_var;
                    }
                    st_add_fn_instr(active_function, conv);
                }
            } else { // booleans cannot be compared this way
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == and || type == or) {
            new_dst = (op_t)sem_new_tmp_var(dt_boolean);
            if(src1->dtype == dt_boolean && src2->dtype == dt_boolean) { // both operands must be boolean
                // OK
            } else { // everything else if prohibited
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == not) {
            new_dst = (op_t)sem_new_tmp_var(dt_boolean);
            i.src2 = NULL;
            if(src1->dtype == dt_boolean) { // operand must be boolean
                // OK
            } else { // everything else if prohibited
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        }
    }
    *dst = i.dst = new_dst;
    if(st_add_fn_instr(active_function, i) != 0) {
        fprintf(stderr, "ERR: Internal error.\n");
        return 99;
    }
    return 0;
}

int sem_generate_mov(op_t src, op_t dst) {
    struct instr i;
    i.type = mov;
    if(src->dtype == dst->dtype) {
        i.src1 = src;
    } else if(src->dtype == dt_int && dst->dtype == dt_double) {
        struct instr conv;
        conv.type = int_to_dbl;
        conv.src1 = src;
        conv.src2 = NULL;
        if(active_function) {
            conv.dst = (op_t)sem_new_tmp_var(dt_double); // TODO
            st_add_fn_instr(active_function, conv); // TODO
        }
        i.src1 = conv.dst;
    }
    i.dst = dst;
    i.src2 = NULL;
    if(active_function)
        st_add_fn_instr(active_function, i);
    return 0;
}