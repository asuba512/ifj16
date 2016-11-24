/**
 * @file 	   sym_table.h
 * @brief      Header file of table of symbols module.
 * @authors    Paliesek Jakub
 */

#ifndef SYM_TABLE_INC
#define SYM_TABLE_INC 1

#include <stdbool.h>
#include "infinite_string.h"
#include "ial.h"
#include "token.h"
#include "ilist.h"

typedef enum {
	dt_double,
	dt_int,
	dt_String,
	dt_boolean,
	t_void
} datatype;

typedef enum {
	var,
	func
} var_func;

typedef enum {
	local,
	global,
	helper
} scope;

typedef union {
	double d_val;
	int i_val;
	string_t s_val;
	bool b_val;
} var_value;

// generic struct for global var, local var, literal, helper var - will be always casted in interpret (except jmp, jmpif)
// its purpose is to simplify the 3AC to work with 
typedef struct operand {
	scope sc;
	datatype dtype;
} *op_t;

typedef struct instr {
	instr_type_t type;
	op_t src1, src2, dst;
	struct instr *next;
} *instr_t;

/** 
 * @brief Global class table
 */
typedef struct class_table {
	bst_node_t root; ///< Root node
} *class_table_t;

typedef struct global_helper_var {
	struct operand op;
	bool initialized;
	var_value val; ///< value
} *glob_helper_var_t; // literals and global tmp variables are stored in this struct (created during generation of global var initialization code)

struct global_helper_var_arr {
	int length, max_length;
	glob_helper_var_t arr;
} glob_helper_vars;

/** 
 * @brief Pointer to class entry in global class table
 */
typedef struct class {
	//string_t id;
	bst_node_t root; ///< Root node
} *class_t;

/**
 * @brief      Represents a local variable or argument in the table of local variables of function.
 * 
 * A new set of local variable instances is pushed to the stack whenever a function is called.
 */
typedef struct local_var {
	struct operand op;
	string_t id;
	int index; ///< index in array of variable instances in function context, unique within one function
} *local_var_t;

/**
 * @brief Entry in table of members of class
 * 
 * Either static function or static variable.
 */
typedef struct class_memb {
	struct operand op;
	bool initialized; ///< indicates whether static variable was initialized or not, not used by function
	var_value val; ///< value of global variable, not used by functions
	var_func type; ///< indicates whether entry represents function or variable
	int arg_count; ///< argument count, not used by static variable
	int _max_arg_count;
	int var_count; ///< local variable count, including arguments, not used by static variable
	local_var_t *arg_list; ///< array of pointers to argument entries in local variable table
	                       ///< ordered by index in function header
	                       ///< not used by static variable
	bst_node_t local_sym_table_root; ///< root node of local table of symbols, not used by static variable
	instr_t instr_list;
	instr_t instr_list_end;
} *class_memb_t;


/**
 * @brief      Represents an instance of local variable or function argument.
 * 
 * A new instance is created and pushed to the stack whenever a function is called and is popped when functions returns from control.
 */
typedef struct local_var_inst {
	bool initialized; ///< indicates whether local variable was initialized or not
	var_value val; ///< value of local variable instance
} local_var_inst_t;

/**
 * @brief      Represents one "context" of funcion. Contains an array of local variable values.
 * 
 * A new context is created and pushed to the context stack whenever a function is called. Once completed, context is popped from the stack.
 */
typedef struct stackframe {
	struct stackframe *next; ///< pointer to next function context on the stack
	local_var_inst_t *vars; ///< array of variable instances in current context
	instr_t ret_addr; ///< store next instruction here before calling function
	var_value eax; // "register", where return value is stored after returning from function
} *stackframe_t;

typedef struct instr_list {
	instr_t head;
	instr_t tail;
} instr_list_t;

instr_list_t glob_instr_list;

// just to avoid one malloc
struct class_table ctable;

/**
 * Global variable - table of classes. Will point to variable above.
 */
class_table_t classes;

/**
 * Global variable - top of function call stack.
 */
stackframe_t call_stack_top;


void init_class_table();
int insert_class(string_t id, class_t *target);
int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt);
int st_add_fn_arg(class_memb_t fn, datatype dt, string_t id);
int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id);
local_var_t st_fn_add_tmpvar(class_memb_t fn, datatype dt, string_t id);
class_t st_getclass(string_t id);
class_memb_t st_getmemb(class_t c, string_t id);
glob_helper_var_t add_global_helper_var(struct token t, bool initialized);
local_var_t st_get_loc_var(class_memb_t m, string_t id);
int st_add_fn_instr(class_memb_t fn, struct instr i);
int st_add_glob_instr(struct instr i);

void st_destroy_all();
void destroy_class(bst_node_t c);
void destroy_class_memb(bst_node_t m);
void destroy_loc_var(bst_node_t v);

#endif
