#include "interpret.h"
#include "sym_table.h"
#include "ilist.h"
#include "ifj16_class.h"
#include "parser.h" // error_number
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gc.h"
#include <math.h>
#include <errno.h>
#include <limits.h>

//main function, it is processing the instructions on the instruction tape, returns 0 if everything was OK, returns non-zero value if any error occurs(depending on the type of error)
int inter(instr_t I){

	int arg_counter = 0;//argument counter in variable array of function
	stackframe_t new_frame = NULL;//variable for new frame
	local_var_inst_t new_arg;//variable for new argument
	bool init;//for testing of initialization of variables ??
	datatype dtype;//for getting datatype
	bool *init_src1;//bool pointers for setting the inicialization of values 
	bool *init_src2;
	bool *init_dest;
	int src1_i_value = 0;//for work with int value
	int src2_i_value = 0;//for work with int value
	double src1_d_value = 0.0;//for work with double value
	double src2_d_value = 0.0;//for work with double value
	bool src1_b_value = false;//for work with src1 bool value
	bool src2_b_value = false;//for work with src2 bool value
	var_value *value1;//for getting src1
	var_value *value2;//for getting src2
	var_value *dest;//for getting dst
	char arr[50];//array for instructions converting to string

	while(I) { //while I != NULL (end of instruction tape)
		
		switch (I->type)//switch for getting the instruction type and provide the needed operations
		{

			case halt:
				fprintf(stderr,"Error! Missing return in non-void function.\n");
				clear_frames();
				return 8;
				break;

			case sframe://create new function frame
				arg_counter=0;
				new_frame=malloc(sizeof(struct stackframe));//allocating memory for new frame
				if(new_frame==NULL) { 
					fprintf(stderr,"Error at memory allocation!\n");
					clear_frames();
					return 99;
				}
				new_frame->vars=calloc((sizeof(struct local_var_inst)*(((class_memb_t)I->src1)->var_count)),1);//allocate memory for array of variablec(including fn arguments) 
				if(new_frame->vars==NULL) {
					free(new_frame);
					fprintf(stderr,"Error at memory allocation!\n");
					clear_frames();
					return 99;
				}
				break;

			case push:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));//check the initialization of variable(argument)
				if(init != true) {
					fprintf(stderr,"Error! Working with uninitialized variable.\n");
					free(new_frame->vars);
					free(new_frame);
					clear_frames();
					return 8; 
				}
				new_arg.initialized=init;//set initialization of argument 
				new_arg.val=(*value1);//set value of argument
				((new_frame->vars)[arg_counter])=new_arg;//add argument at the end of fn variable array
				arg_counter++;
				break;

			case call:
				new_frame->ret_addr=I->next;//return adress is saved to new frame
				inter_stack_push(new_frame);//push sframe to stack
				I=((class_memb_t)I->dst)->instr_list;//jump to instruction list of called function
				continue;

			case ret:                
				if(I->src1==NULL) { //void, do eax sa nedava nic
					I=inter_stack.top->ret_addr;//continue at return adress
					inter_stack_pop();//removing actual frame
					continue;
				}
				else {//non-void
					init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));//vo value1 navratova hodnota
					if(init != true) {
						return 8;
					}
					inter_stack.top->next->eax=(*value1);//return value is saved to eax(variable for storing return value) of calling function
					I=inter_stack.top->ret_addr;//continue at return adress
					inter_stack_pop();//removing actual frame
					continue;
				}

			case movr://moves return value of function to its destination in symbol table
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				*dest = (inter_stack.top)->eax;
				(*init_dest) = true;
				break;

			case i_d_r:
				inter_stack.top->eax.d_val=(double)inter_stack.top->eax.i_val;//retyping int to double
				break;

			case jmp:
				I=(instr_t)I->dst;
				break;
			
			case jmpif:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
				}
				if(value1->b_val){//checking that statement is true
					I=(instr_t)I->dst;
				}
				break;

			case jmpifn:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){ 
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
				}
				if(!(value1->b_val)){//checking that statement is false
					I=(instr_t)I->dst;
				}
				break;
				
			case label:
				break;

			//instructions for converting datatypes, when converting to str sprintf is used
			case int_to_dbl:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
				}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				(*init_dest)=true;
				(*dest).d_val=(double)(*value1).i_val;
				break;                
			
			case int_to_str:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
				}
				sprintf(arr, "%d", value1->i_val);
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				(*init_dest)=true;
				dest->s_val = str_init(arr);
				if(dest->s_val == NULL){
					fprintf(stderr, "ERR: Internal error.\n");
					free(new_frame->vars);
					free(new_frame);
					clear_frames();
					return 99;
				}
				break;
			
			case bool_to_str:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
				}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				(*init_dest)=true;
				if(value1->b_val) {
					dest->s_val=str_init("true");
					if(dest->s_val == NULL){
						fprintf(stderr, "ERR: Internal error.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 99;
					}
				}
				else {
					dest->s_val=str_init("false");
					if(dest->s_val == NULL){
						fprintf(stderr, "ERR: Internal error.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 99;
					}
				}
				break;

			case dbl_to_str:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
				}
				sprintf(arr, "%g", value1->d_val);
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				(*init_dest)=true;
				dest->s_val = str_init(arr);
				if(dest->s_val == NULL){
					fprintf(stderr, "ERR: Internal error.\n");
					free(new_frame->vars);
					free(new_frame);
					clear_frames();
					return 99;
				}
				break;
			
			//arithmetic instructions
			case add:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)//getting the datatype of operands, semantic analysis provides that they will be the same type
					{        //the same control is made at every arithemtic instruction
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							break;
						default: 
							break;

					}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				switch(dtype)
					{
						case dt_double:
							(*dest).d_val=src1_d_value+src2_d_value;
							break;
						case dt_int:
							(*dest).i_val=src1_i_value+src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;
			
			case sub:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							break;
						default: 
							break;

					}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				switch(dtype)
					{
						case dt_double:
							(*dest).d_val=src1_d_value-src2_d_value;
							break;
						case dt_int:
							(*dest).i_val=src1_i_value-src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;
			
			case imul:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							break;
						default: 
							break;

					}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				switch(dtype)
					{
						case dt_double:
							(*dest).d_val=src1_d_value*src2_d_value;
							break;
						case dt_int:
							(*dest).i_val=src1_i_value*src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;
			
			case idiv:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double://here division by zero is good(becaus of doubles)
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							if(src2_i_value==0) {//checking division by zero
								fprintf(stderr,"Error! Dividing by 0.\n");
								clear_frames();
								return 9;
							}
							break;
						default: 
							break;

					}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				switch(dtype)
					{
						case dt_double:
							(*dest).d_val=src1_d_value/src2_d_value;
							break;
						case dt_int:
							(*dest).i_val=src1_i_value/src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;
			
			case conc:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
					} 
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2)); 
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						free(new_frame->vars);
						free(new_frame);
						clear_frames();
						return 8;
					}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				dest->s_val = str_init(value1->s_val->data);
				if(dest->s_val == NULL){
					fprintf(stderr, "ERR: Internal error.\n");
					free(new_frame->vars);
					free(new_frame);
					clear_frames();
					return 99;
				}
				str_cat(dest->s_val, value2->s_val);
				(*init_dest) = true;
				break;
			
			//comparisions
			case eql:
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)//getting the dataype of operands too, like in arithmetic isntructions
					{		 //it is the same at every comparision
						case dt_double:
							dest->b_val = (*value1).d_val == (*value2).d_val;
							break;
						case dt_int:
							dest->b_val = (*value1).i_val == (*value2).i_val;
							break;
						case dt_boolean:
							dest->b_val = value1->b_val == value2->b_val;
						default:
							break;
					}
				(*init_dest)=true;
				break;

			case neq:
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							dest->b_val = src1_d_value != src2_d_value;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							dest->b_val = src1_i_value != src2_i_value;
							break;
						case dt_boolean:
							dest->b_val = value1->b_val != value2->b_val;
						default:
							break;
					}
				(*init_dest)=true;
				break;


			case gre:
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							dest->b_val = src1_d_value > src2_d_value;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							dest->b_val = src1_i_value > src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;

			case less:
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							dest->b_val = src1_d_value < src2_d_value;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							dest->b_val = src1_i_value < src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;

			case geq:
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							dest->b_val = src1_d_value >= src2_d_value;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							dest->b_val = src1_i_value >= src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;

			case leq:
			   init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				switch(dtype)
					{
						case dt_double:
							src1_d_value=(*value1).d_val;
							src2_d_value=(*value2).d_val;
							dest->b_val = src1_d_value <= src2_d_value;
							break;
						case dt_int:
							src1_i_value=(*value1).i_val;
							src2_i_value=(*value2).i_val;
							dest->b_val = src1_i_value <= src2_i_value;
							break;
						default:
							break;
					}
				(*init_dest)=true;
				break;

			//logical operations
			case or:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				src1_b_value=(*value1).b_val; //here we use only bool values do do not need to check datatypes
											  //same at every logical instruction
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				src2_b_value=(*value2).b_val;

				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				dest->b_val = src1_b_value || src2_b_value;
				(*init_dest)=true;
				break;

			
			case and:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				src1_b_value=(*value1).b_val;
				
				init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
					if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				src2_b_value=(*value2).b_val;

				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				dest->b_val = src1_b_value && src2_b_value;
				(*init_dest)=true;
				break;

			case not:
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init!=true) {
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
					}
				src1_b_value=(*value1).b_val;

				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				dest->b_val = !src1_b_value;
				(*init_dest)=true;
				break;

			case mov://assigment,moving value from source to destination
				init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
				if(init != true){
						fprintf(stderr,"Error! Working with uninitialized variable.\n");
						clear_frames();
						return 8;
				}
				init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
				*dest=*value1;
				(*init_dest)=true;
				break;

			//built in functions for strings, getting arguments from the variable array of function, saving return values to this array(one index above argument with highest index) 
			//errors are checked too
			case r_str:
				(inter_stack.top)->vars[0].val.s_val=ifj16_readString();
				if(error_number==99){
					fprintf(stderr, "ERR: Internal error.\n");
					clear_frames();
					return error_number;
				}
				(inter_stack.top)->vars[0].initialized = true;
				break;
			
			case r_int:
				(inter_stack.top)->vars[0].val.i_val=ifj16_readInt();
				if(error_number==7) {
					fprintf(stderr, "ERR: Given input is not valid integer.\n");
					clear_frames();
					return error_number;
				}
				if(error_number==99){
					fprintf(stderr, "ERR: Internal error.\n");
					clear_frames();
					return error_number;
				}
				if(error_number == 42){
					fprintf(stderr, "Input error - Invalid value for an int.\n");
					clear_frames();
					return 7;
				}
				(inter_stack.top)->vars[0].initialized = true;
				break;
			
			case r_dbl:
				(inter_stack.top)->vars[0].val.d_val=ifj16_readDouble();
				if(error_number==7) {
					fprintf(stderr, "ERR: Given input is not valid floating point number.\n");
					clear_frames();
					return error_number;
				}
				if(error_number==99){
					fprintf(stderr, "ERR: Internal error.\n");
					clear_frames();
					return error_number;
				}
				(inter_stack.top)->vars[0].initialized = true;
				break;
			//built in functions for strings, getting arguments from the variable array of function, saving return values to this array(one index above argument with highest index) 
			//errors are checked too
			case len:
				(inter_stack.top)->vars[1].val.i_val=ifj16_length((inter_stack.top)->vars[0].val.s_val);
				(inter_stack.top)->vars[1].initialized = true;
				break;
			
			case subs:
				(inter_stack.top)->vars[3].val.s_val=ifj16_substr((inter_stack.top)->vars[0].val.s_val,(inter_stack.top)->vars[1].val.i_val,(inter_stack.top)->vars[2].val.i_val);
				if(error_number==99){
					fprintf(stderr, "ERR: Internal error.\n");
					clear_frames();
					return error_number;
				}
				if(error_number==10){
					fprintf(stderr,"ERR: Index out of bounds.\n");
					clear_frames();
					return error_number;
				}
				if(error_number == 42){ // special error code to distinguish error messages
					fprintf(stderr, "ERR: Cannot create substring with negative length\n");
					clear_frames();
					return 10;
				}
				(inter_stack.top)->vars[3].initialized = true;
				break;
			
			case cmp:
				(inter_stack.top)->vars[2].val.i_val=ifj16_compare((inter_stack.top)->vars[0].val.s_val,(inter_stack.top)->vars[1].val.s_val);
				(inter_stack.top)->vars[2].initialized = true;
				break;
			
			case findstr:
				(inter_stack.top)->vars[2].val.i_val=ifj16_search((inter_stack.top)->vars[0].val.s_val,(inter_stack.top)->vars[1].val.s_val);
				(inter_stack.top)->vars[2].initialized = true;
				break;
			
			case sortstr:
				(inter_stack.top)->vars[1].val.s_val=ifj16_sort((inter_stack.top)->vars[0].val.s_val);
				if(error_number==99){
					fprintf(stderr, "ERR: Internal error.\n");
					clear_frames();
					return error_number;
				}
				(inter_stack.top)->vars[1].initialized = true;
				break;
			
			case prnt:
				ifj16_print((inter_stack.top->vars)[0].val.s_val);
				break;

		}

		I=I->next;//jump to next next instruction
	}
	return 0;
}
// you should aways check whether variable is initialized by return value
// to mark dst initialized use what "iniliazed" parameter returns
// when decoding literal, "initialized" is undefined
bool decode_address(op_t op, var_value **target, datatype *dtype, bool **initialized) {
	class_memb_t glob;
	local_var_t loc;
	glob_helper_var_t hlpr;
	if(op->sc == global) {
		glob = (class_memb_t)op;
		*dtype = glob->op.dtype;
		*target = &(glob->val);
		*initialized = &(glob->initialized);
		return glob->initialized;
	} else if(op->sc == helper) {
		hlpr = (glob_helper_var_t)op;
		*dtype = hlpr->op.dtype;
		*target = &(hlpr->val);
		*initialized = &(hlpr->initialized);
		return hlpr->initialized; // this ain't just literal anymore ...
	} else if(op->sc == local) {
		loc = (local_var_t)op;
		*dtype = loc->op.dtype;
		*target = &(((inter_stack.top->vars)[loc->index]).val);
		*initialized = &(((inter_stack.top->vars)[loc->index]).initialized);
		return ((inter_stack.top->vars)[loc->index]).initialized;
	} else {
		*target = NULL;
		*initialized = NULL;
	}
	return false;
}


//functions for working with interpret stack
void inter_stack_init(){
	inter_stack.top=NULL;
}

void inter_stack_push(stackframe_t context){
	context->next=inter_stack.top;
	inter_stack.top=context;
}

void inter_stack_pop(){
	if(inter_stack.top!=NULL){
		stackframe_t tmp;
		tmp=inter_stack.top;
		inter_stack.top=inter_stack.top->next;
		free(tmp->vars);
		free(tmp);
	}
}

//function for clearing all frames if an error occurs
void clear_frames() {
	stackframe_t previous = inter_stack.top;
	if(!previous) return;
	for(stackframe_t item = previous->next; item != NULL; item = item->next) {
		free(previous->vars);
		free(previous);
		previous = item;
	}
	free(previous->vars);
		free(previous);
}
