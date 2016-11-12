#include "sym_table.h"
#include "infinite_string.h"


class_t active_class;
class_memb_t active_function;
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