#include "token.h"
#include <stdio.h>

#define FIRST_PASS (pass_number == 1)
#define SECOND_PASS (pass_number == 2)

token_t t;
FILE *fd;
int lexerror;
int pass_number; // either first or second

int c_list();
int c_memb(); 
int c_memb1();
int memb_list();
int type();
int c_memb_func();
int c_memb2();
int fn_def_plist();
int fn_def_plist1();
int par_def();
int fn_body();
int stat_com();
int stat_list();
int id();
int id1();
int stat();
int as_ca();
int opt_assign();
int assign();
int ret_val();
int fn_plist();
int fn_plist1();
int val_id();
