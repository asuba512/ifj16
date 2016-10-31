typedef enum instruction_type{
	halt, //return 0
	//arithemtic - maybe each for all types of int,double combination
	add,
	sub,
	mul,
	div,
	//concatonate
	concatonate,
	//comparing - maybe each for all typet of int,double combination
	equal,
	not_equal,
	greater,
	less,
	greater_equal,
	less_equal,
	//logical
	log_or,
	log_and,
	log_not,
	//assign
	assign,
	//built in functions
	print, //todo - maybe each for int,double,string
	read_int,
	read_double,
	read_string,
	length,
	substr,
	compare,
	find,
	sort,
	//jumps //todo
	label,
	jump,
	if_jump,
	//for functions
	push,
	pop,
	f_return //retrn from function
}instruction_type;
