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
 * @brief Pointer to class entry in global class table
 */
typedef struct class_table {
	bst_node_t root; ///< Root node
} class_table_t;


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
	}; ///< value of global variable
	datatype type; ///< return value for functions, datatype for variable
	int arg_count; ///< argument count
	local_var_t *arg_list; ///< array of pointers to argument entries in local variable table
	                       ///< ordered by index in function header
	bool initialized; ///< indicates whether variable was initialized or not
	bst_node_t local_sym_table_root; ///< root node of local table of symbols
} *class_memb_t;


/**
 * @brief      Represents an instance of local variable or function argument.
 * 
 * A new instance is created and pushed to the stack whenever a function is called and is popped when functions returns from control.
 */
typedef struct local_var_inst {
	bool initialized; ///< indicates whether variable was initialized or not
	union {
		double d_val;
		int i_val;
		string_t s_val;
		bool b_val;
	}; ///< value of local variable instance
	struct local_var_inst *next; ///< pointer to the next instance of variable on the stack
} *local_var_inst_t;


/**
 * @brief      Represents a local variable or argument in the table of local variables table of function.
 * 
 * A new instance is created and pushed to the stack whenever a function is called and is popped when functions returns from control.
 */
typedef struct local_var_t {
	datatype type; ///< return value for functions, datatype for variable
	local_var_inst_t stack_top; ///< points to the top of variable instances stack
} *local_var_t;


/** Global variable - highest level table */
class_table_t class_table;

#endif