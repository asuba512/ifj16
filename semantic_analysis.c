#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"
#include "token.h"
#include <stdio.h>

int sem_new_class(string_t id) {
    class_t c;
    int err = insert_class(id, &c);
    if (err == 0) {
        active_class = c;
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Can't redefine class \"%s\".\n", id->data);
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    active_class = NULL; // causes segfault (intentionally) if program continues  ilegally
    return err;
} // OK

int sem_add_member_active_class(var_func member_type) {
    class_memb_t new_memb;
    int err = st_insert_class_memb(active_class, &new_memb, sem_tmp_data.id, member_type, sem_tmp_data.dt);
    if (err == 0) {
        if(member_type == func) {
            active_function = new_memb;
        }
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Class member redefinition.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    active_function = NULL; // causes segfault (intentionally) if program continues ilegally
    return err;
} // OK

int sem_add_arg_active_fn() {
    int err = st_add_fn_arg(active_function, sem_tmp_data.dt, sem_tmp_data.id);
    if (err == 0) {
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Function argument with same identifier already exists.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    return err;
} // OK

void sem_mark_sec_pass(string_t id) {
    class_memb_t var = st_getmemb(active_class, id);
    var->second_pass = true;
}

void sem_set_active_class(string_t id) {
    active_class = st_getclass(id); // cant cause err (I think)
} // OK

void sem_set_active_fn(string_t id) {
    active_function = st_getmemb(active_class, id); // cant cause err (I think)
} // OK

int sem_prec_reduction() {
    return 42;
} // LOL ???

void sem_search(string_t class_id, string_t memb_id) {
    void *symbol = NULL;
    if (class_id == NULL) {
        if(!outside_func) { // local scope has more priority inside function
            symbol = st_get_loc_var(active_function, memb_id);
            if(!symbol)
                symbol = st_getmemb(active_class, memb_id);
        } else { // outside function, we search only for global identifiers
            symbol = st_getmemb(active_class, memb_id);
        }
    } else {
        class_t class = st_getclass(class_id);
        if (class) {
            symbol = st_getmemb(class, memb_id);
        }
    }
    sem_id_decoded.ptr = symbol;
    setIsFunFlag(symbol);
} // OK -> never causes error by itself

void setIsFunFlag(void *symbol) {
    if(symbol != NULL) {
        class_memb_t memb = (class_memb_t)symbol;
        if(memb->op.sc == global)
            if(memb->type == func)
                sem_id_decoded.isFun = true;
            else
                sem_id_decoded.isFun = false;
        else
            sem_id_decoded.isFun = false;
    } else
        sem_id_decoded.isFun = false;
} // OK

int sem_new_loc_var(datatype dt, string_t id) {
    class_memb_t m = st_getmemb(active_class, id);
    if((m && !(m->type == func)) || !(m)) {
        int err = st_add_fn_locvar(active_function, dt, id);
        if (err == 0) {
            return 0;
        } else if(err == 3) {
            fprintf(stderr, "ERR: Function local variable with same identifier already exists.\n");
        } else if(err == 99) {
            fprintf(stderr, "ERR: Internal error.\n");
        }
        return err;
    } else {
        fprintf(stderr, "ERR: Cannot define local variable '%s'. A function already exists with same identifier in current context.\n", id->data);
        return 3;
    }
    
} // OK

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
} // OK -> only causes internal err

int sem_generate_arithm(instr_type_t type, op_t src1, op_t src2, op_t *dst) {
    op_t new_var = NULL, new_dst = NULL; // new_var = result of conversion, new_dst destination of effective instruction
    token_t t;  // for generating conversion tmp var
    struct instr i, conv; // i - main instruction, conv - helper conversion instruction
    int err = 0; // catches error codes
    // just default values
    i.type = type;
    i.src1 = src1;
    i.src2 = src2;
    *dst = NULL;
    // dst will be set at the end because we don't know its datatype yet
    if(src1->sc == global && (((class_memb_t)(src1))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    } else if (src2 && src2->sc == global && (((class_memb_t)(src2))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    }
    if(src1->dtype == dt_String || (src2 && src2->dtype == dt_String)) {
        if(type == add) {
            i.type = conc; // concatenation of strings and addicion are different instructions
            NEW_STRING(new_dst) // result of concatenation
            if(!new_dst) INTERNAL_ERR
            // conversion and further modifications in instruction are needed, when one of operands is not a string  
            if(src1->dtype != dt_String || src2->dtype != dt_String) {   
                NEW_STRING(new_var) // converted operand   
                if(!new_var) INTERNAL_ERR
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
                INSTR(conv)
                if(err) INTERNAL_ERR
            }
            
        } else {
            fprintf(stderr, "ERR: Invalid operation with strings.\n");
            return 4;
        }
    } else {
        if(type == add || type == sub || type == imul || type == idiv) {
            if(isNum(src1) && isNum(src2)) {
                if(src1->dtype == dt_int && src2->dtype == dt_int)
                    NEW_INT(new_dst)
                else {
                    NEW_DOUBLE(new_dst)
                    if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                        NEW_DOUBLE(new_var) // converted operand
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
                        INSTR(conv)
                        if(err) INTERNAL_ERR
                    }
                }
                if(!new_dst) INTERNAL_ERR
            } else {
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == eql || type == neq) { // either numbers or booleans can be tested for equivalence
            NEW_BOOLEAN(new_dst)
            if(!new_dst) INTERNAL_ERR
            if(isNum(src1) && isNum(src2)) {
                if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                    NEW_DOUBLE(new_var) // converted operand
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
                    INSTR(conv)
                    if(err) INTERNAL_ERR
                }
            } else if(src1->dtype == dt_boolean && src2->dtype == dt_boolean) {
                // OK
            } else { // combination of number and boolean is prohibited
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == leq || type == geq || type == gre || type == less) { 
            NEW_BOOLEAN(new_dst)
            if(!new_dst) INTERNAL_ERR
            if(isNum(src1) && isNum(src2)) {
                if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                    NEW_DOUBLE(new_var) // converted operand
                    if(!new_var) INTERNAL_ERR
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
                    INSTR(conv)
                    if(err) INTERNAL_ERR
                }
            } else { // booleans cannot be compared this way
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == and || type == or) {
            NEW_BOOLEAN(new_dst)
            if(!new_dst) INTERNAL_ERR
            if(src1->dtype == dt_boolean && src2->dtype == dt_boolean) { // both operands must be boolean
                // OK
            } else { // everything else if prohibited
                fprintf(stderr, "ERR: Incompatible types of operands.\n");
                return 4;
            }
        } else if(type == not) {
            NEW_BOOLEAN(new_dst)
            if(!new_dst) INTERNAL_ERR
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
    INSTR(i)
    if(err) INTERNAL_ERR
    return 0;
} // OK, trivial, evident

int sem_generate_mov(op_t src, op_t dst) {
    struct instr i;
    token_t t; // for generating conversion tmp var
    int err = 0; // catches error codes
    i.type = mov;
    if(src->sc == global && (((class_memb_t)(src))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    } else if(dst->sc == global && (((class_memb_t)(dst))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as destination of assignment.\n");
        return 4;
    } else if (!dst) { // undefined dst
        fprintf(stderr, "ERR: Undefined variable used as destination of assignment.\n");
        return 3;
    } 
    if(src->dtype == dst->dtype) {
        i.src1 = src;
    } else if(src->dtype == dt_int && dst->dtype == dt_double) {
        struct instr conv;
        conv.type = int_to_dbl;
        conv.src1 = src;
        conv.src2 = NULL;
        NEW_DOUBLE(conv.dst)
        if(!conv.dst) INTERNAL_ERR
        INSTR(conv)
        if(err) INTERNAL_ERR
        i.src1 = conv.dst;
    } else {
        fprintf(stderr, "ERR: Incompatible types of operands.\n");
        return 4;
    }
    i.dst = dst;
    i.src2 = NULL;
    INSTR(i)
    if(err) INTERNAL_ERR
    return 0;
} // OK

// generates 'sframe' instruction
int sem_generate_prepare(class_memb_t fn) {
    if(fn->op.sc == local) {
        fprintf(stderr, "ERR: Trying to call function, which is variable.\n");
        return 4;
    } else if(fn->op.sc == global && fn->type == var) {
        fprintf(stderr, "ERR: Trying to call function, which is variable.\n");
        return 4;
    }
    struct instr i;
    i.type = sframe;
    i.src1 = (op_t)fn;
    i.src2 = i.dst = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    calling_function = fn;
    return 0;
} // OK

int sem_generate_push(class_memb_t called_fn, op_t arg) {
    if(arg_counter == called_fn->arg_count) {
        fprintf(stderr, "ERR: Too many arguments\n");
        return 4;
    }
    token_t t; // macro
    struct instr i;
    struct instr conv;
    i.type = push;
    i.src2 = i.dst = NULL;
    i.src1 = arg;
    if(arg->sc == global && (((class_memb_t)(arg))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 4;
    }
    if(arg->dtype == ((called_fn->arg_list)[arg_counter])->op.dtype || (arg->dtype == dt_int && ((called_fn->arg_list)[arg_counter])->op.dtype == dt_double)) { 
        // OK
		if(arg->dtype == dt_int && ((called_fn->arg_list)[arg_counter])->op.dtype == dt_double){
			NEW_DOUBLE(conv.dst)
            if(!conv.dst) INTERNAL_ERR
            conv.src1 = arg;
            conv.src2 = NULL;
            conv.type = int_to_dbl;
            if(st_add_fn_instr(active_function, conv)) INTERNAL_ERR
            i.src1 = conv.dst;
		}
    } else {
        fprintf(stderr, "ERR: Incompatible type of argument.\n");
        return 4;
    }
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    arg_counter++;
    return 0;
}

void sem_rst_argcount() {
    arg_counter = 0;
}

bool sem_args_ok(class_memb_t called_fn) {
    if((called_fn->arg_count != arg_counter))
        fprintf(stderr,"ERR: Too few arguments.\n");
    return called_fn->arg_count == arg_counter;
}

int sem_generate_call(class_memb_t called_fn) {
    struct instr i;
    i.type = call;
    i.dst = (op_t)called_fn;
    i.src1 = i.src2 = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_generate_movr(class_memb_t called_fn, op_t dst) {
    struct instr i;
    i.type = movr;
    if (!dst) { // undefined dst
        fprintf(stderr, "ERR: Undefined variable used as destination of assignment.\n");
        return 3;
    } else if(dst->sc == global && (((class_memb_t)(dst))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as destination of assignment.\n");
        return 4;
    }
    if(called_fn->op.dtype == dst->dtype) {
        // OK
    } else if(called_fn->op.dtype == dt_int && dst->dtype == dt_double) {
        struct instr conv;
        conv.type = i_d_r;
        conv.dst = conv.src2 = conv.src1 = NULL;
        // no operands, just convert value in register
        if(st_add_fn_instr(active_function, conv)) INTERNAL_ERR
    } else {
        fprintf(stderr, "ERR: Incompatible types of operands.\n");
        return 4;
    }
    i.dst = dst;
    i.src1 = i.src2 = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_generate_jmpifn(op_t src) {
    struct instr i;
    i.type = jmpifn;
    if(src->dtype != dt_boolean) {
        fprintf(stderr, "ERR: Non-boolean value used as condition.\n");
        return 4;
    }
    i.src2 = i.dst = NULL;
    i.src1 = src;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_generate_label() {
    struct instr i;
    i.type = label;
    i.src1 = i.src2 = i.dst = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_generate_jmp(op_t dst) {
    struct instr i;
    i.type = jmp;
    i.src1 = i.src2 = NULL;
    i.dst = dst;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_set_jmp_dst(instr_t i, op_t dst) {
    i->dst = dst;
    return 0;
}

int sem_generate_ret(op_t src) {
    struct instr i;
    i.type = ret;
    i.src2 = i.dst = NULL;
    if(active_function->op.dtype != t_void && !src) {
        fprintf(stderr,"ERR: Non-void-function must return a value.\n");
        return 4;
    } else if(active_function->op.dtype == t_void && src) {
        fprintf(stderr,"ERR: Void-function can't return a value.\n");
        return 4;
    } else if((active_function->op.dtype == t_void && !src) || active_function->op.dtype == src->dtype) {
        i.src1 = src;
    } else if(active_function->op.dtype == dt_double && src->dtype == dt_int) {
        struct instr conv;
        conv.type = int_to_dbl;
        conv.src1 = src;
        conv.src2 = NULL;
        conv.dst = (op_t)sem_new_tmp_var(dt_double);
        if(!conv.dst || st_add_fn_instr(active_function, conv)) INTERNAL_ERR
        i.src1 = conv.dst;
    } else {
        fprintf(stderr,"ERR: Function does not return a value of compatible type.\n");
        return 4;
    }
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

int sem_generate_halt() {
    struct instr i;
    i.type = halt;
    i.dst = i.src1 = i.src2 = NULL;
    if (st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

// returns op_t because we want to use the result of conversion immediately
op_t sem_generate_conv_to_str(op_t op) {
    struct instr i;
    int err; // catching errors
    i.src1 = op;
    i.src2 = NULL;
    i.dst = (op_t)sem_new_tmp_var(dt_String);
    if (!i.dst) return NULL;
    switch(op->dtype) {
        case dt_int:
            i.type = int_to_str; break;
        case dt_boolean:
            i.type = bool_to_str; break;
        case dt_double:
            i.type = dbl_to_str; break;
        default:
            break;
    }
    INSTR(i)
    if(err) return NULL;
    return i.dst;
}
