#include "sym_table.h"
#include "infinite_string.h"


class_t active_class;
class_memb_t active_function;
struct temp_data {
    string_t id;
    datatype dt;
} sem_tmp_data;


int sem_new_class(string_t id);
int sem_add_member_active_class(var_func member_type);
int sem_add_arg_active_fn();