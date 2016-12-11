#include "sym_table.h"
#include "infinite_string.h"

/** Global variable to store pointer to class in whose body we are in. */
class_t active_class;
/** Global variable to store pointer to function in whose body we are in. */
class_memb_t active_function;
/** Global var to store pointer to function we're calling (for processing arguments purposes). */
class_memb_t calling_function;
/** Counter of processed arguments when calling function */
int arg_counter;
/** Stores result of precedence analysis */
op_t precedence_result;
/** Flag indicating whether we are currently inside or outside function */
bool outside_func;
/** Stores data before submitting new symbol to ST. */
struct temp_data {
    string_t id;
    datatype dt;
} sem_tmp_data;
/** Stores result of searching for symbols in sem_search() */
struct fq {
    bool isFun;
    void* ptr;
} sem_id_decoded;


int sem_new_class(string_t id);
int sem_add_member_active_class(var_func member_type);
int sem_add_arg_active_fn();
void sem_set_active_class(string_t id);
void sem_set_active_fn(string_t id);
void sem_search(string_t class_id, string_t memb_id);
void setIsFunFlag(void *symbol);
int sem_new_loc_var(datatype dt, string_t id);
int sem_generate_arithm(instr_type_t type, op_t src1, op_t src2, op_t *dst);
int sem_generate_mov(op_t src, op_t dst);
int sem_generate_prepare(class_memb_t fn);
int sem_generate_push(class_memb_t called_fn, op_t arg);
void sem_rst_argcount();
bool sem_args_ok(class_memb_t);
int sem_generate_movr(class_memb_t called_fn, op_t dst);
int sem_generate_call(class_memb_t);
int sem_generate_jmpifn(op_t src);
int sem_generate_label();
int sem_generate_jmp(op_t);
int sem_set_jmp_dst(instr_t, op_t);
int sem_generate_ret(op_t src);
int sem_generate_halt();
op_t sem_generate_conv_to_str(op_t op);
void sem_mark_sec_pass(string_t id);
int add_head();

/** Helper condition to determine whether operand is a number (double/int) */
#define isNum(x) (x->dtype == dt_double || x->dtype == dt_int)

/** Shorthand for throwing an internal error */
#define INTERNAL_ERR {fprintf(stderr, "ERR: Internal error.\n"); return 99;}

/** These create a new helper variable (local/global - based on outside_func global flag) */
#define NEW_STRING(d) if (!outside_func) d = (op_t)sem_new_tmp_var(dt_String); else {t.type = token_string; d = (op_t)add_global_helper_var(t, false);}
#define NEW_DOUBLE(d) if (!outside_func) d = (op_t)sem_new_tmp_var(dt_double); else {t.type = token_double; d = (op_t)add_global_helper_var(t, false);}
#define NEW_BOOLEAN(d) if (!outside_func) d = (op_t)sem_new_tmp_var(dt_boolean); else {t.type = token_boolean; d = (op_t)add_global_helper_var(t, false);}
#define NEW_INT(d) if (!outside_func) d = (op_t)sem_new_tmp_var(dt_int); else {t.type = token_int; d = (op_t)add_global_helper_var(t, false);}

/* This generates new instruction (local/global - based on outside_func global flag) */
#define INSTR(i) if(outside_func) { err = st_add_glob_instr( i ); } else { err = st_add_fn_instr(active_function, i ); }