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
	sframe, // prepare function stackframe (before pushing)
	call, // save next address on stack and jump to first instruction of dst
	//jumps //todo
	label,
	jmp, // jump to dst
	jmpifn, // jump to dst if not src1
	jmpif, // jump to dst if src1
	//for functions
	push,
	ret, //return from function (will assign entire union to special place)
	movr, // move from register to dst 
	i_d_r, // convert register i_val to d_val
	prnt, // print string stored in variable index 0 on stackframe
	r_dbl,
	r_str,
	r_int,
	len
} instr_type_t;

#endif
