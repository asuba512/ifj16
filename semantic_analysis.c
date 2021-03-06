#include "sym_table.h"
#include "semantic_analysis.h"
#include "infinite_string.h"
#include "token.h"
#include "ifj16_class.h" // STR() macro
#include <stdio.h>

/** Tries to create a new class with given id in ST. On success, returns 0,
    otherwise corresponding error is returned and error message is printed
    on stderr */
int sem_new_class(string_t id) {
    class_t c;
    int err = insert_class(id, &c);
    if (err == 0) {
        active_class = c; // update active class indicator
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Can't redefine class \"%s\".\n", id->data);
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    active_class = NULL; // causes segfault (intentionally) if program continues  ilegally
    return err;
}

/** Tries to create a new class member in 'active_class' (see header file)
    with given id in ST. On success, returns 0, otherwise corresponding error
    is returned and error message is printed on stderr. Needed info is retrieved
    from parameter 'member_type' and global structure 'sem_tmp_data' (see header file). */
int sem_add_member_active_class(var_func member_type) {
    class_memb_t new_memb;
    int err = st_insert_class_memb(active_class, &new_memb, sem_tmp_data.id, member_type, sem_tmp_data.dt);
    if (err == 0) {
        if(member_type == func) {
            active_function = new_memb; // update active function indicator
        }
        return 0;
    } else if(err == 3) {
        fprintf(stderr, "ERR: Class member redefinition.\n");
    } else if(err == 99) {
        fprintf(stderr, "ERR: Internal error.\n");
    }
    active_function = NULL; // causes segfault (intentionally) if program continues ilegally
    return err;
}

/** Tries to add argument to 'active_function' (see header file). On success, returns 0,
    otherwise corresponding error is returned and error message is printed on stderr.
    Needed info is retrieved from global structure 'sem_tmp_data' (see header file). */
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
}

/** Marks static var with given 'id' as "reached" by second pass */
void sem_mark_sec_pass(string_t id) {
    class_memb_t var = st_getmemb(active_class, id);
    var->second_pass = true;
}

/** Sets class with given 'id' as active (the one, between whose
    brackets {} the current token is). */
void sem_set_active_class(string_t id) {
    active_class = st_getclass(id); 
}

/** Sets function with given 'id' as active (the one, between whose
    brackets {} the current token is). */
void sem_set_active_fn(string_t id) {
    active_function = st_getmemb(active_class, id); 
}

/** Tries to search for symbol (suitable for current context, 
    considering active_function and active_class), with given
    intifier(s) in ST. On success, a global variable 'sem_id_decoded.ptr'
    is set to corresponding element (function or variable).
    Otherwise NULL is returned. */
void sem_search(string_t class_id, string_t memb_id) {
    void *symbol = NULL;
    if (class_id == NULL) { // when there's no class selected, we assume active one
        if(!outside_func) { // local scope has higher priority than global one
            symbol = st_get_loc_var(active_function, memb_id);
            if(!symbol) // if we had no luck, we try to search global identifier
                symbol = st_getmemb(active_class, memb_id);
        } else { // outside function, we search only for global identifiers
            symbol = st_getmemb(active_class, memb_id);
        }
    } else { // with given class id, we don't do any assumptions
        class_t class = st_getclass(class_id);
        if (class) { // if class is found, the search for its member
            symbol = st_getmemb(class, memb_id);
        }
    }
    sem_id_decoded.ptr = symbol;
    setIsFunFlag(symbol);
}

/** Sets global flag to indicate whether the identifier we searched for
    was function or variable */
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
}

/** Tries to add a new local variable with given identifier and datatype
    into local ST of 'active_function'. On success, 0 is returned, otherwise
    corresponding error is returned and error is printed to stderr. */
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
}

/** Tries to add a new helper variable with given datatype
    into list in local ST of 'active_function'. On success, pointer
    to newly created variable is returned, otherwise NULL signalizing
    internal err. */
local_var_t sem_new_tmp_var(datatype dt) {
    local_var_t tmpvar = st_fn_add_tmpvar(active_function, dt);
    if (tmpvar == NULL) {
        fprintf(stderr, "ERR: Internal error.\n");
        return NULL;
    }
    return tmpvar;
}

/** Called with reduction steps in precedence analysis. Checks, whether
    operands are type compatible (and generates corresponding conversion
    instructions if needed). Function identifiers always cause semantic error.
    Returns 0 on success, otherwise corresponding number is returned. */
int sem_generate_arithm(instr_type_t type, op_t src1, op_t src2, op_t *dst) {
    op_t new_var = NULL, new_dst = NULL; // new_var = result of conversion, new_dst destination of effective instruction
    token_t t;  // for generating conversion tmp var (the function takes token as parameter)
    struct instr i, conv; // i - main instruction, conv - helper conversion instruction
    int err = 0; // catches error codes
    // just default values
    i.type = type;
    i.src1 = src1;
    i.src2 = src2;
    *dst = NULL;  // dst will be set at the end because we don't know its datatype yet
    if(src1->sc == global && (((class_memb_t)(src1))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    } else if (src2 && src2->sc == global && (((class_memb_t)(src2))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    }
    if(src1->dtype == dt_String || (src2 && src2->dtype == dt_String)) { // one operand is string, we're doing concatenation
        if(type == add) {
            i.type = conc; // concatenation of strings and addicion are different instructions
            NEW_STRING(new_dst) // result of concatenation
            if(!new_dst) INTERNAL_ERR
            // conversion and further modifications in instruction are needed, when one of operands is not a string  
            if(src1->dtype != dt_String || src2->dtype != dt_String) { // one operand is not a string yet, we need to generate conversion
                NEW_STRING(new_var) // converted operand   
                if(!new_var) INTERNAL_ERR
                conv.src2 = NULL; 
                if(src1->dtype != dt_String) {
                    switch(src1->dtype) { // choose appropriate conversion
                        case dt_double: conv.type = dbl_to_str; break;
                        case dt_int: conv.type = int_to_str; break;
                        case dt_boolean: conv.type = bool_to_str; break;
                        default: break;
                    }
                    conv.src1 = src1;
                    i.src1 = new_var;
                } else {
                    switch(src2->dtype) { // choose appropriate conversion
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
            if(isNum(src1) && isNum(src2)) { // both operands are int/double
                if(src1->dtype == dt_int && src2->dtype == dt_int)
                    NEW_INT(new_dst) // both were int, no conversion
                else {
                    NEW_DOUBLE(new_dst)
                    if((src1->dtype == dt_double && src2->dtype == dt_int) || (src1->dtype == dt_int && src2->dtype == dt_double)){
                        // one operand is double and other is int
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
                        INSTR(conv) // generate conversion instruction
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
                    // if one of operands was double and other was int, generate conversion
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
            NEW_BOOLEAN(new_dst) // inequality comparisons only take numbers and generate boolean
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
            } else { // everything else is prohibited
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
    INSTR(i) // finally generate effective arithmetic instruction
    if(err) INTERNAL_ERR
    return 0;
}

/** Checks type compatibility, creates conversion if applicable, and generates
    mov instuction, a.k.a. assignment. Return 0 on success, otherwise corresponding
    error code is returned. */
int sem_generate_mov(op_t src, op_t dst) {
    struct instr i;
    token_t t; // for generating conversion tmp var (macro takes token)
    int err = 0; // catches error codes
    i.type = mov;
    if(src->sc == global && (((class_memb_t)(src))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as variable.\n");
        return 3;
    } else if(dst->sc == global && (((class_memb_t)(dst))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as destination of assignment.\n");
        return 3;
    } else if (!dst) { // undefined dst
        fprintf(stderr, "ERR: Undefined variable used as destination of assignment.\n");
        return 3;
    } 
    if(src->dtype == dst->dtype) { // compatible operands
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
    INSTR(i) // finally generate assignment instruction
    if(err) INTERNAL_ERR
    return 0;
}

/** Generates instruction sframe (tells interpreter which function is going
    to be called, so that it is able to allocate enough memory). Also performs
    check if symbol is with given identifier is function, otherwise error
    is returned. */
int sem_generate_prepare(class_memb_t fn) {
    if(fn->op.sc == local) {
        fprintf(stderr, "ERR: Trying to call function, which is variable.\n");
        return 3;
    } else if(fn->op.sc == global && fn->type == var) {
        fprintf(stderr, "ERR: Trying to call function, which is variable.\n");
        return 3;
    }
    struct instr i;
    i.type = sframe;
    i.src1 = (op_t)fn;
    i.src2 = i.dst = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    calling_function = fn; // remember function we are calling in global variable
    return 0;
}

/** Generates 'push' instruction (used to copy argument's value onto stack). 
    Performs type check and returns corresponding error code. */
int sem_generate_push(class_memb_t called_fn, op_t arg) {
    // fistly, check if we do not push more parameters than called function takes
    if(arg_counter == called_fn->arg_count) {
        fprintf(stderr, "ERR: Too many arguments\n");
        return 4;
    }
    token_t t; // for macro 'NEW_DOUBLE'
    struct instr i;
    struct instr conv;
    i.type = push;
    i.src2 = i.dst = NULL;
    i.src1 = arg;
    // only variable element can be pushed
    if(arg->sc == global && (((class_memb_t)(arg))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as function argument.\n");
        return 3;
    }
    if(arg->dtype == ((called_fn->arg_list)[arg_counter])->op.dtype || (arg->dtype == dt_int && ((called_fn->arg_list)[arg_counter])->op.dtype == dt_double)) { 
        // OK
		if(arg->dtype == dt_int && ((called_fn->arg_list)[arg_counter])->op.dtype == dt_double){
            // if we're pushing int and function expects double, generate conversion first
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

/** Resets count of pushed arguments of function call that is processed at the moment */
void sem_rst_argcount() {
    arg_counter = 0;
}

/** Called at the end of (.., .., ..) sequence when calling function,
    to check whether sufficient arguments count was given. */
bool sem_args_ok(class_memb_t called_fn) {
    if((called_fn->arg_count != arg_counter))
        fprintf(stderr,"ERR: Too few arguments.\n");
    return called_fn->arg_count == arg_counter;
}

/** Generates 'call' instruction after processing of arguments is done. */
int sem_generate_call(class_memb_t called_fn) {
    struct instr i;
    i.type = call;
    i.dst = (op_t)called_fn;
    i.src1 = i.src2 = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

/** Generates instruction to assign return value of called function to 'dst'.
    Performs type check and generates appropriate conversion, if applicable.
    Return 0 on success, otherwise error code is returned. */
int sem_generate_movr(class_memb_t called_fn, op_t dst) {
    struct instr i;
    i.type = movr;
    if (!dst) { // undefined dst
        fprintf(stderr, "ERR: Undefined variable used as destination of assignment.\n");
        return 3;
    } else if(dst->sc == global && (((class_memb_t)(dst))->type) == func) {
        fprintf(stderr, "ERR: Function identifier used as destination of assignment.\n");
        return 3;
    }
    if(called_fn->op.dtype == dst->dtype) {
        // OK
    } else if(called_fn->op.dtype == dt_int && dst->dtype == dt_double) {
        // generate additional conversion (special one, just for return value "register")
        struct instr conv;
        conv.type = i_d_r;
        conv.dst = conv.src2 = conv.src1 = NULL; // implicit operand 'eax'
        // no operands, just convert value in "register"
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

/** Generates conditional jump instruction based on 'src'. Performs type check.
    Additional operand 'dst' must be supplied later */
int sem_generate_jmpifn(op_t src) {
    struct instr i;
    i.type = jmpifn;
    if(src->sc == global && ((class_memb_t)src)->type == func) {
        fprintf(stderr, "ERR: Trying to use function identifier as a condition.\n");
        return 3;
    }
    if(src->dtype != dt_boolean) {
        fprintf(stderr, "ERR: Non-boolean value used as condition.\n");
        return 4;
    }
    i.src2 = i.dst = NULL;
    i.src1 = src;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

/** Generates dummy label instruction as a destination of jumps */
int sem_generate_label() {
    struct instr i;
    i.type = label;
    i.src1 = i.src2 = i.dst = NULL;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

/** Generates non-conditional jump instruction. */
int sem_generate_jmp(op_t dst) {
    struct instr i;
    i.type = jmp;
    i.src1 = i.src2 = NULL;
    i.dst = dst;
    if(st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

/** Provides 'dst' operand (destination of a jump) to given instruction. */
int sem_set_jmp_dst(instr_t i, op_t dst) {
    i->dst = dst;
    return 0;
}

/** Generates return instruction, setting return value to 'src'. 
    'src' can be NULL -> function assumes empty return expression.
    Type check of return value is also performed. Return expression 
    of a void-function must be empty. */
int sem_generate_ret(op_t src) {
    struct instr i;
    i.type = ret;
    i.src2 = i.dst = NULL;
    if(active_function->op.dtype != t_void && !src) {
        // trying to return; (without expression) in non-void function
        fprintf(stderr,"ERR: Non-void-function must return a value.\n");
        return 4;
    } else if(active_function->op.dtype == t_void && src) {
        // opposite case - trying to return a value in void-function
        fprintf(stderr,"ERR: Void-function can't return a value.\n");
        return 4;
    } else if((active_function->op.dtype == t_void && !src) || active_function->op.dtype == src->dtype) {
        i.src1 = src; // compatible return types
    } else if(active_function->op.dtype == dt_double && src->dtype == dt_int) {
        // generating additional conversion
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

/** Function to append a 'halt' instruction. When interpreter reaches this,
    it signalizes that the called function is missing return statement
    although it is non-void. */
int sem_generate_halt() {
    struct instr i;
    i.type = halt;
    i.dst = i.src1 = i.src2 = NULL;
    if (st_add_fn_instr(active_function, i)) INTERNAL_ERR
    return 0;
}

/** Function to generate conversion instruction of non-string operand
    to string. Returns string operand, so we can use it immediately.
    Is only used when evaluating concatenation in print statement,
    so there's no support for global operand result. */
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

/** Finds Main.run() function and appends call instruction to the very
    end of global instruction tape. If problem occurs, prints error message
    and returns corresponding error code. */
int add_head() {
	struct instr i;
	string_t str;
	i.type = sframe;
	STR("Main")
	class_t c = st_getclass(str);
	if(!c) {
		fprintf(stderr,"ERR: Missing 'Main' class.\n");
		return 3;
	}
	STR("run")
	i.src1 = (op_t)st_getmemb(c, str);
	if(!i.src1) {
		fprintf(stderr,"ERR: Missing 'Main.run()' function.\n");
		return 3;
	}
	if(((class_memb_t)(i.src1))->op.dtype != t_void) {
		fprintf(stderr,"ERR: 'Main.run()' must be a void-funcion.\n");
		return 3;
	} else if(((class_memb_t)(i.src1))->arg_count) {
		fprintf(stderr,"ERR: 'Main.run()' must not have any arguments.\n");
		return 3;
	}
	i.dst = i.src2 = NULL;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}
	i.dst = i.src1;
	i.src1 = NULL;
	i.type = call;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}	
	i.type = label;
	i.dst = i.src1 = i.src2 = NULL;
	if(st_add_glob_instr(i)) {
		fprintf(stderr, "ERR: Internal error.\n");
		return 99;
	}
	return 0;
}
