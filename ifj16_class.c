#include "sym_table.h"
#include "ifj16_class.h"
#include "ial.h"
#include <stdlib.h>
#include <stdio.h>

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
    i.src1 =  (op_t)st_fn_add_tmpvar(readInt, dt_int, str_init("^result")); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readInt, i);

    // ifj16.readDouble
    class_memb_t readDouble;
    st_insert_class_memb(ifj16, &readDouble, str_init("readDouble"), func, dt_double);
    i.type = r_dbl;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readDouble, dt_double, str_init("^result")); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);

    // ifj16.readString
    class_memb_t readString;
    st_insert_class_memb(ifj16, &readString, str_init("readString"), func, dt_String);
    i.type = r_str;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readString, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readString, dt_String, str_init("^result")); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readString, i);

    // ifj16.length
    class_memb_t length;
    st_insert_class_memb(ifj16, &length, str_init("length"), func, dt_int);
    st_add_fn_arg(length, dt_String, str_init("s")); // arg (index 0)
    i.type = len;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(length, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(length, dt_int, str_init("^result")); // local var to store result (index 1)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(length, i);

    // ifj16.substr
    class_memb_t substr;
    st_insert_class_memb(ifj16, &substr, str_init("substr"), func, dt_String);
    st_add_fn_arg(substr, dt_String, str_init("s")); // arg (index 0)
    st_add_fn_arg(substr, dt_int, str_init("i")); // arg (index 1)
    st_add_fn_arg(substr, dt_int, str_init("n")); // arg (index 2)
    i.type = subs;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(substr, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(substr, dt_String, str_init("^result")); // local var to store result (index 3)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(substr, i);

    // ifj16.compare
    class_memb_t compare;
    st_insert_class_memb(ifj16, &compare, str_init("compare"), func, dt_int);
    st_add_fn_arg(compare, dt_String, str_init("s1")); // arg (index 0)
    st_add_fn_arg(compare, dt_String, str_init("s2")); // arg (index 1)
    i.type = cmp;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(compare, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(compare, dt_int, str_init("^result")); // local var to store result (index 2)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(compare, i);

    // ifj16.find
    class_memb_t find;
    st_insert_class_memb(ifj16, &find, str_init("find"), func, dt_int);
    st_add_fn_arg(find, dt_String, str_init("s")); // arg (index 0)
    st_add_fn_arg(find, dt_String, str_init("search")); // arg (index 1)
    i.type = findstr;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(find, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(find, dt_int, str_init("^result")); // local var to store result (index 2)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(find, i);

    // ifj16.sort
    class_memb_t sort;
    st_insert_class_memb(ifj16, &sort, str_init("sort"), func, dt_int);
    st_add_fn_arg(sort, dt_String, str_init("s")); // arg (index 0)
    i.type = sortstr;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(sort, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(sort, dt_int, str_init("^result")); // local var to store result (index 1)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(sort, i);

    return 0;
}

void ifj16_print(string_t s) {
    printf("%s", s->data);
}

int ifj16_readInt() {
    return 42;
}

double ifj16_readDouble() {
    return 41.999;
}

string_t ifj16_readString() {
    return NULL;
}

int ifj16_length(string_t s) {
    return s->length;
}

string_t ifj16_substr(string_t s, int i, int n) {
    (void)s;
    (void)i;
    (void)n;
    return NULL;
}

int ifj16_compare(string_t s1, string_t s2) {
    return str_compare(s1,s2);
}

int ifj16_search(string_t s, string_t search) {
    return BMA_index(s,search);
}

string_t ifj16_sort(string_t s) {
    shell_sort(s);
    return s;
}