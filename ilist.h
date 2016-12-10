#ifndef ILIST_INC
#define ILIST_INC 1

typedef enum instruction_type {
	halt, //return void
	//arithmetic
	add,
	sub,
	imul,
	idiv, // added 'i' to avoid conflict with stdlib
	conc, //concatenation
	//comparison
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
	label, // jump target
	jmp, // jump to dst
	jmpifn, // jump to dst if not src1
	jmpif, // jump to dst if src1
	// function calling
	sframe, // prepare function stackframe (before pushing)
	call, // save next instruction address on stack and jump to first instruction of dst
	push, // copy parameter to stack
	ret, //return from function (will assign entire union to special "register")
	movr, // move from register to dst 
	i_d_r, // convert register i_val to d_val
	// built-in functions (ifj16 class)
	prnt, // print string stored in variable index 0 on stackframe
	r_dbl, // ifj16.readDouble
	r_str, // ifj16.readString
	r_int, // ifj16.readInt
	len, // ifj16.length
	subs, // ifj16.substr
	cmp, // ifj16.compare
	findstr, // ifj16.find
	sortstr // ifj16.sort
} instr_type_t;

#endif
