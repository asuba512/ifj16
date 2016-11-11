#ifndef ILIST_INC
#define ILIST_INC 1

typedef enum instruction_type {
	halt, //return void
	//arithmetic - maybe each for all types of int,double combination
	add,
	sub,
	imul,
	idiv, // added 'i' to avoid conflict with stdlib
	//concatenate
	conc,
	//comparing - maybe each for all typet of int,double combination
	eql,
	neq,
	gre,
	less,
	geq,
	leq,
	//logical
	or,
	and,
	not,
	//assign
	mov, // avoided symbol conflict (there's already function called "assign()" in parser)
	//conversion
	int_to_dbl,
	int_to_str,
	bool_to_str,
	dbl_to_str,
	//built in functions
	print, //todo - maybe each for int,double,string
	cmp,
	sframe, // prepare function stackframe (before pushing)
	call,
	//jumps //todo
	label,
	jmp,
	jmpif,
	//for functions
	push,
	ret //return from function
} instr_type_t;

typedef struct instr {
	instr_type_t type;
	op_t src1, src2, dst;
	struct instr *next;
} *instr_t;

#endif