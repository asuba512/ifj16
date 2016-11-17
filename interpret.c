#include "interpret.h"
#include "sym_table.h"
#include <stdbool.h>

// you should aways check whether variable is initialized by return value
// to mark dst initialized use what "iniliazed" parameter returns
// when decoding literal, "initialized" is undefined
bool decode_address(op_t op, var_value **target, datatype *dtype, bool **initialized) {
    class_memb_t glob;
    local_var_t loc;
    glob_helper_var_t helper;
    if(op->sc == global) {
        glob = (class_memb_t)op;
        *dtype = glob->dtype;
        *target = &(glob->val);
        *initialized = &(glob->initialized);
        return glob->initialized;
    } else if(op->sc == helper) {
        helper = (glob_helper_var_t)op;
        *dtype = helper->dtype;
        *target = &(helper->val);
        *initialized = &(helper->initialized);
        return helper->initialized; // this ain't just literal anymore ...
    } else if(op->sc == local) {
        loc = (local_var_t)op;
        *dtype = loc->dtype;
        *target = &(((context_stack_top->vars)[loc->index]).val);
        *initialized = &(((context_stack_top->vars)[loc->index]).initialized);
        return ((context_stack_top->vars)[loc->index]).initialized;
    }
    return false;
}
