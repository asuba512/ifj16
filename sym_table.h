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

typedef enum {
	dt_double,
	dt_int,
	dt_String,
	dt_boolean
} datatype;


/** 
 * @brief Global class table
 */
typedef struct class_table {
	bst_node_t root; ///< Root node
} *class_table_t;


/** 
 * @brief Pointer to class entry in global class table
 */
typedef struct class {
	bst_node_t root; ///< Root node
} *class_t;


/**
 * @brief Entry in table of members of class
 * 
 * Either static function or static variable.
 */
typedef struct class_memb {
	union {
		double d_val;
		int i_val;
		string_t s_val;
		bool b_val;
	}; ///< value of global variable, not used by functions
	datatype type; ///< return value for functions, datatype for variable
	int arg_count; ///< argument count, not used by static variable
	int var_count ///< local variable count, including arguments, not used by static variable
	local_var_t *arg_list; ///< array of pointers to argument entries in local variable table
	                       ///< ordered by index in function header
	                       ///< not used by static variable
	bool initialized; ///< indicates whether static variable was initialized or not, not used by function
	bool defined; ///< indicates whether variable or function was defined
	bst_node_t local_sym_table_root; ///< root node of local table of symbols, not used by static variable
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
 * @brief      Represents a local variable or argument in the table of local variables of function.
 * 
 * A new set of local variable instances is pushed to the stack whenever a function is called.
 */
typedef struct local_var {
	datatype type; ///< return value for functions, datatype for variable
	int index; ///< index in array of variable instances in function context, unique within one function
} *local_var_t;


/**
 * @brief      Represents one "context" of funcion. Contains an array of local variable values.
 * 
 * A new context is created and pushed to the context stack whenever a function is called. Once completed, context is popped from the stack.
 */
typedef struct fn_context {
	struct fn_context *next; ///< pointer to next function context on the stack
	local_var_inst_t *vars; ///< array of variable instances in current context
} *fn_context_t;


/**
 * Global variable - table of classes.
 */
class_table_t class_table;

/**
 * Global variable - top of function context stack.
 */
fn_context_t context_stack_top;

#endif