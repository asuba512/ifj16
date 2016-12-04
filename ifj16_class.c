#include "sym_table.h"
#include "ifj16_class.h"
#include "ial.h"
#include "parser.h"
#include "infinite_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

extern int error_number;

int populate_sym_table() {
    string_t str;
    string_t str_result;
    if(!(str_result = str_init("^result"))) {
        fprintf(stderr, "ERR: Internal error.\n"); return 99;
    }
    struct instr i;
    class_t ifj16;
    STR("ifj16")
    insert_class(str, &ifj16);
    
    // ifj16.print
    class_memb_t print;
    STR("print")
    st_insert_class_memb(ifj16, &print, str, func, t_void);
    st_add_fn_arg(print, dt_String, str);
    i.type = prnt;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(print, i);
    i.type = ret;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(print, i);
    print_fn = print;

    // ifj16.readInt
    class_memb_t readInt;
    STR("readInt")
    st_insert_class_memb(ifj16, &readInt, str, func, dt_int);
    i.type = r_int;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readInt, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readInt, dt_int); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readInt, i);

    // ifj16.readDouble
    class_memb_t readDouble;
    STR("readDouble")
    st_insert_class_memb(ifj16, &readDouble, str, func, dt_double);
    i.type = r_dbl;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readDouble, dt_double); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readDouble, i);

    // ifj16.readString
    class_memb_t readString;
    STR("readString")
    st_insert_class_memb(ifj16, &readString, str, func, dt_String);
    i.type = r_str;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(readString, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(readString, dt_String); // local var to store result (index 0)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(readString, i);

    // ifj16.length
    class_memb_t length;
    STR("length")
    st_insert_class_memb(ifj16, &length, str, func, dt_int);
    st_add_fn_arg(length, dt_String, str); // arg (index 0)
    i.type = len;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(length, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(length, dt_int); // local var to store result (index 1)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(length, i);

    // ifj16.substr
    class_memb_t substr;
    STR("substr")
    st_insert_class_memb(ifj16, &substr, str, func, dt_String);
    st_add_fn_arg(substr, dt_String, str); // arg (index 0)
    STR("substr1")
    st_add_fn_arg(substr, dt_int, str); // arg (index 1)
    STR("substr2")
    st_add_fn_arg(substr, dt_int, str); // arg (index 2)
    i.type = subs;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(substr, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(substr, dt_String); // local var to store result (index 3)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(substr, i);

    // ifj16.compare
    class_memb_t compare;
    STR("compare")
    st_insert_class_memb(ifj16, &compare, str, func, dt_int);
    st_add_fn_arg(compare, dt_String, str); // arg (index 0)
    STR("compare1")
    st_add_fn_arg(compare, dt_String, str); // arg (index 1)
    i.type = cmp;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(compare, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(compare, dt_int); // local var to store result (index 2)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(compare, i);

    // ifj16.find
    class_memb_t find;
    STR("find")
    st_insert_class_memb(ifj16, &find, str, func, dt_int);
    st_add_fn_arg(find, dt_String, str); // arg (index 0)
    STR("find1")
    st_add_fn_arg(find, dt_String, str); // arg (index 1)
    i.type = findstr;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(find, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(find, dt_int); // local var to store result (index 2)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(find, i);

    // ifj16.sort
    class_memb_t sort;
    STR("sort")
    st_insert_class_memb(ifj16, &sort, str, func, dt_String);
    st_add_fn_arg(sort, dt_String, str); // arg (index 0)
    i.type = sortstr;
    i.dst = i.src1 = i.src2 = NULL;
    st_add_fn_instr(sort, i);
    i.type = ret;
    i.src1 =  (op_t)st_fn_add_tmpvar(sort, dt_int); // local var to store result (index 1)
    i.dst = i.src2 = NULL;
    st_add_fn_instr(sort, i);

    return 0;
}

void ifj16_print(string_t s) {
    printf("%s", s->data);
}

int ifj16_readInt() {
    string_t s=str_init("");
    if(s==NULL){
        error_number=99;
    }
    int c;
    int counter=0;
    long int num;
    char * pEnd=NULL;
    while((c=getchar()) != EOF){
    	if(counter == 0){
    		if(isspace(c) || c=='\n'){
    			error_number=7;
    		}
    	}
    	if(c=='\n') break;
            if((str_addchar(s, c)) == 99){
                error_number=99;
            }
         counter++;
    	}
    num=strtol(s->data,&pEnd,10);
    if(errno == ERANGE || num > INT_MAX || num < INT_MIN){
        error_number=42;
    }
    if (*pEnd != '\0') {
    	error_number=7;
    }
    return (int)num;
}

double ifj16_readDouble() {
	string_t s=str_init("");
    if(s==NULL){
        error_number=99;
    }
    int c;
    int counter=0;
    double num;
    char * pEnd=NULL;
    while((c=getchar()) != EOF){
    	if(counter == 0){
    		if(isspace(c) || c=='\n'){
    			error_number=7;
    		}
    	}
    	if(c=='\n') break;
    	    if((str_addchar(s, c)) == 99){
                error_number=99;
            }
         counter++;
    	}
    num=strtod(s->data,&pEnd);
    if (*pEnd != '\0') {
    	error_number=7;
    }
    return num;

}

string_t ifj16_readString() {
    string_t s=str_init("");
    if(s==NULL){
        error_number=99;
    }
    int c;
    while((c=getchar()) != EOF){
    	if(c=='\n') break;
            if((str_addchar(s, c)) == 99){
                error_number=99;
            }
    	}
    return s;
}

int ifj16_length(string_t s) {
    return s->length;
}

string_t ifj16_substr(string_t s, int i, int n) {
    if(i<0 || i > s->length || (i+n)>s->length){
    	error_number=10;
    }


    string_t s2=str_init("");
    if(s2==NULL){
        error_number=99;
    }
    for(int j=0;j<n;j++){
        if((str_addchar(s2, s->data[i+j])) == 99){
                error_number=99;
            }
 	}
 	return s2;
}

int ifj16_compare(string_t s1, string_t s2) {
    int cmp=str_compare(s1,s2);
    if(cmp==0){
    	return 0;
    }
    else if(cmp>0){
    	return 1;
    }
    else {
    	return -1;
    }
}

int ifj16_search(string_t s, string_t search) {
    return BMA_index(s,search);
}

string_t ifj16_sort(string_t s) {
    string_t s2 = str_init(s->data);
    if(s2==NULL){
        error_number=99;
    }
    shell_sort(s2);
    return s2;
}