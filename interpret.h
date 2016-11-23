#include <stdbool.h>
#include "sym_table.h"


typedef struct inter_stack{
	stackframe_t top;
}inter_stack_t;

inter_stack_t inter_stack;//stack for function frames

int inter(instr_t I);
bool decode_address(op_t op, var_value **target, datatype *dtype, bool **initialized); // returns whether variable was initialized or not
void inter_stack_init();
void inter_stack_push();
void inter_stack_pop();
void clear_frames();