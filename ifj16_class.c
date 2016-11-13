#include "sym_table.h"
#include "ifj16_class.h"
#include <stdlib.h>

int populate_sym_table() {
    struct instr i;
    class_t ifj16;
    insert_class(str_init("ifj16"), &ifj16);
    
    // ifj16.print
    class_memb_t print;
    st_insert_class_memb(ifj16, &print, str_init("print"), func, t_void);
    st_add_fn_arg(print, dt_String, str_init("str"));
    i.type = prnt;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(print, i);
    i.type = ret;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(print, i);
    print_fn = print;

    return 0;
}