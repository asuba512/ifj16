#ifndef SYM_TABLE_INC
#define SYM_TABLE_INC 1

#include <stdbool.h>
#include "infinite_string.h"
#include "ial.h"
#include "token.h"
#include "ilist.h"

#define ARG_CHUNK 5 // chunk for adding space to array of pointers to function arguments

/** Represents datatype in ST */
typedef enum {
	dt_double,
	dt_int,
	dt_String,
	dt_boolean,
	t_void
} datatype;

/** Represents information whether symbol is function or var (needed in static symbols) */
typedef enum {
	var,
	func
} var_func;

/** Indicates scope of a symbol. In most places local variables, class members
	and helper variables are used interchangeably, but sometimes we need to
	distinct them. Before casting op_t (see below) to any of these, we need to look at this
	enum first. */
typedef enum {
	local,
	global,
	helper
} scope;

/** Stores actual value of variable. */
typedef union {
	double d_val;
	int i_val;
	string_t s_val;
	bool b_val;
} var_value;

// generic struct for global var, local var, literal, helper var - will be always casted in interpret (except jmp, jmpif)
// its purpose is to simplify the 3AC to work with 
// before casting to another struct, we need to look at 'sc'
// is always the first element of symbol structure
// "A pointer to a structure object, suitably converted, points to its initial member.", ISO C Standard
typedef struct operand {
	scope sc;
	datatype dtype;
} *op_t;

/** Represents instruction in global/local instruction tape */
typedef struct instr {
	instr_type_t type;
	// operands are op_t by default, but are always casted to one of these: class_memb_t, local_var_t, glob_helper_var_t, instr_t
	op_t src1, src2, dst; 
	struct instr *next;
} *instr_t;

/** Global class table (Level1 ST) */
typedef struct class_table {
	bst_node_t root; ///< Root node
} *class_table_t;

/** Helper variable, that is created as a product storing literal or in precedence analysis for global var initialization */
typedef struct global_helper_var {
	struct operand op; // common part with other symbols
	bool initialized;
	var_value val;
	struct global_helper_var *next; // stored in linked list
} *glob_helper_var_t;

/** List of global helpers variables */
struct global_helper_varz {
	glob_helper_var_t head;
	glob_helper_var_t tail;
} glob_helper_vars;

/** Pointer to class entry in global class table. */
typedef struct class {
	bst_node_t root; // Root node of Level2 ST (class member table)
} *class_t;

/** Represents a local variable or argument in the table of local variables (Level3 ST) of a function.
	Is also used to store helper variables (in linked list) created during precedence analysis. */
typedef struct local_var {
	struct operand op; // common part with other symbols
	int index; // index in array of variable instances in function context, unique within one function
	struct local_var *next; // for helper vars stored in linkedlist, not used by regular vars or arguments
} *local_var_t;

/** Entry in table of members of class (Level2 ST), either static function or static variable.
	These elements share one structure representation, because we need to to store them in
	a single BST. */
typedef struct class_memb {
	struct operand op; // common part with other symbols
	bool initialized; // indicates whether static variable was initialized or not, not used by functions
	var_value val; // value of global variable, not used by functions
	var_func type; // indicates whether entry represents function or variable
	bool second_pass; // indicates whether variable was already processed in second pass (applies on vars only)
	int arg_count; // argument count, not used by variables
	int _max_arg_count; // indicates currently allocated space for arg_list array
	int var_count; // local variable count, including arguments, not used by static variable
	local_var_t *arg_list; // array of pointers to argument entries in local variable table
	                       // ordered by index in function prototype
	                       // not used by static variable
	local_var_t helper_vars; // stack-like linked list of helper variables created during precedence analysis inside function
	bst_node_t local_sym_table_root; // root node of local table of symbols (Level3 ST), not used by static variables
	instr_t instr_list; // instruction tape head
	instr_t instr_list_end; // instruction tape tail
} *class_memb_t;


/** Represents an instance of local variable, function argument or helper variable. 
	New instances are created when function is called during interpretaion. 
	This is where interpret stores values of local vars. */
typedef struct local_var_inst {
	bool initialized; // indicates whether local variable was initialized or not
	var_value val; // value of local variable instance
} local_var_inst_t;

/** Represents one stackframe-like structure where interpreter stores values of local variables,
	return addresses for returning and return values. */
typedef struct stackframe {
	struct stackframe *next; // pointer to next function 'stackframe' on the stack
	local_var_inst_t *vars; // array of variable instances in current context
	instr_t ret_addr; // store next instruction here before calling function
	var_value eax; // "register", where return value is stored after returning from function
} *stackframe_t;

/** Instruction list structure */
typedef struct instr_list {
	instr_t head;
	instr_t tail;
} instr_list_t;

/** Global instruction list */
instr_list_t glob_instr_list;

// just to avoid one malloc
struct class_table ctable;

/** Global variable - table of classes. Will point to variable above. */
class_table_t classes;


void init_class_table();
int insert_class(string_t id, class_t *target);
int st_insert_class_memb(class_t c, class_memb_t *target, string_t id, var_func type, datatype dt);
int st_add_fn_arg(class_memb_t fn, datatype dt, string_t id);
int st_add_fn_locvar(class_memb_t fn, datatype dt, string_t id);
local_var_t st_fn_add_tmpvar(class_memb_t fn, datatype dt);
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
