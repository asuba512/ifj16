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
	literal
} scope;

/** 
 * @brief Global class table
 */
typedef struct class_table {
	bst_node_t root; ///< Root node
} *class_table_t;

// generic struct for global var, local var, literal, helper var - will be always casted (except jmp, jmpif)
typedef struct operand {
	scope sc;
	datatype dt;
	instr_t instr; // jump target
} op_t;

typedef struct literal {
	scope sc; ///< for interpreter
	datatype dtype; ///< datatype of literal
	union {
		double d_val;
		int i_val;
		string_t s_val;
		bool b_val;
	}; ///< value of literal
} *literal_t;

struct literal_arr {
	int length, max_length;
	literal_t arr;
} literals;

/** 
 * @brief Pointer to class entry in global class table
 */
typedef struct class {
	string_t id;
	bst_node_t root; ///< Root node
} *class_t;

/**
 * @brief      Represents a local variable or argument in the table of local variables of function.
 * 
 * A new set of local variable instances is pushed to the stack whenever a function is called.
 */
typedef struct local_var {
	/** Common part with class_memb */
	scope sc; ///< for interpreter
	datatype dtype; ///< datatype of variable
	string_t id;
	/** end of common part */
	int index; ///< index in array of variable instances in function context, unique within one function
} *local_var_t;

/**
 * @brief Entry in table of members of class
 * 
 * Either static function or static variable.
 */
typedef struct class_memb {
	/** Common part with local_var */
	scope sc; ///< for interpreter
	datatype dtype; ///< return value for functions, datatype for variable
	string_t id; // temp
	/** end of common part */
	union {
		double d_val;
		int i_val;
		string_t s_val;
		bool b_val;
	}; ///< value of global variable, not used by functions
	var_func type; ///< indicates whether entry represents function or variable
	int arg_count; ///< argument count, not used by static variable
	int _max_arg_count;
	int var_count; ///< local variable count, including arguments, not used by static variable
	local_var_t *arg_list; ///< array of pointers to argument entries in local variable table
	                       ///< ordered by index in function header
	                       ///< not used by static variable
	bool initialized; ///< indicates whether static variable was initialized or not, not used by function
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
	union {
		double d_val;
		int i_val;
		string_t s_val;
		bool b_val;
	}; ///< value of local variable instance
} local_var_inst_t;

/**
 * @brief      Represents one "context" of funcion. Contains an array of local variable values.
 * 
 * A new context is created and pushed to the context stack whenever a function is called. Once completed, context is popped from the stack.
 */
typedef struct fn_context {
	struct fn_context *next; ///< pointer to next function context on the stack
	local_var_inst_t *vars; ///< array of variable instances in current context
} *fn_context_t;

// just to avoid one malloc
struct class_table ctable;

/**
 * Global variable - table of classes. Will point to variable above.
 */
class_table_t classes;

/**
 * Global variable - top of function context stack.
 */
fn_context_t context_stack_top;

void init_class_table();
int insert_class(string_t id, class_t *target);
int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt);
int st_add_fn_arg(class_memb_t fn, datatype dt, string_t id);
int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id);
class_t st_getclass(string_t id);
class_memb_t st_getmemb(class_t c, string_t id);
literal_t add_literal(struct token t);
local_var_t st_get_loc_var(class_memb_t m, string_t id);
int st_add_fn_instr(class_memb_t fn, struct instr);

#endif
