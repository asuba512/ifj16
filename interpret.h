#include <stdbool.h>
#include "sym_table.h"

bool decode_address(op_t op, var_value **target, datatype *dtype, bool **initialized); // returns whether variable was initialized or not