#include "sym_table.h"
#include "infinite_string.h"


class_t active_class;
class_memb_t active_function;
class_memb_t calling_function;
int arg_counter;
op_t precedence_result;
struct temp_data {
    string_t id;
    datatype dt;
} sem_tmp_data;

struct fq {
    string_t class_id;
    string_t memb_id;
    bool isFun;
    void* ptr;
} sem_id_decoded;


int sem_new_class(string_t id);
int sem_add_member_active_class(var_func member_type);
int sem_add_arg_active_fn();
void sem_set_active_class(string_t id);
void sem_set_active_fn(string_t id);
void sem_search();
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