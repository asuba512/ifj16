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

    // ifj16.readInt
    class_memb_t readInt;
    st_insert_class_memb(ifj16, &readInt, str_init("readInt"), func, dt_int);
    i.type = r_int;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readInt, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readInt, dt_int, str_init("^"));
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readInt, i);

    // ifj16.readDouble
    class_memb_t readDouble;
    st_insert_class_memb(ifj16, &readDouble, str_init("readDouble"), func, dt_double);
    i.type = r_dbl;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readDouble, dt_double, str_init("^"));
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);

    // ifj16.readString
    class_memb_t readString;
    st_insert_class_memb(ifj16, &readString, str_init("readString"), func, dt_String);
    i.type = r_str;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readString, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readString, dt_String, str_init("^"));
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);

    // ifj16.length
    class_memb_t length;
    st_insert_class_memb(ifj16, &length, str_init("length"), func, dt_int);
    st_add_fn_arg(length, dt_String, str_init("s"));
    i.type = len;
    i.src1 = (op_t)st_fn_add_tmpvar(length, dt_int, str_init("^"));
    i.dst = i.src2 = NULL;
    st_add_fn_instr(length, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(length, dt_int, str_init("^"));
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);

    return 0;
}