#include "interpret.h"
#include "sym_table.h"
#include "ilist.h"
#include "ifj16_class.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



int inter(instr_t I){

    int arg_counter;//pocitadlo argumentov v poli premennych
    stackframe_t new_frame;//novy frame
    //stackframe_t old_frame;//frame na odstranenie
    local_var_inst_t new_arg;//novy argument
    bool init;//pre testovanie inicializacie premennych
    datatype dtype;//pre zistenie datoveho typu
    bool *init_src1;//pre nastavenie incializacie
    bool *init_src2;
    bool *init_dest;
    int src1_value;
    int src2_value;
    var_value *value1;//pre src1
    var_value *value2;//pre src2
    var_value *dest;//pre dst

    while(1 && I != NULL) { 
        
        switch (I->type)
        {

            case halt:
                return 0;
                break;

            case sframe:
                arg_counter=0;
                new_frame=malloc(sizeof(struct stackframe));
                if(new_frame==NULL) { 
                    return -1;
                }
                new_frame->vars=malloc(sizeof(struct local_var_inst)*(((class_memb_t)I->src1)->var_count)); 
                if(new_frame->vars==NULL) {
                    free(new_frame);
                    return -1;
                }
                break;

            case push:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true) {
                    return -1; //error
                }
                new_arg.initialized=init;  
                new_arg.val=(*value1);
                ((new_frame->vars)[arg_counter])=new_arg;
                arg_counter++;
                break;

            case call:
                new_frame->ret_addr=I->next;//ulozi sa adresa navratu
                inter_stack_push(new_frame);//push sframe na stack
                I=((class_memb_t)I->dst)->instr_list;//skok na instrukcnu lisitinu volanej funkcie
                continue;

            case ret:
                I=inter_stack.top->ret_addr;//pokracovat odkial sa skocilo
                
                if(I->src1==NULL) { //void, do eax sa nedava nic
                    inter_stack_pop();//zrusenie aktualneho ramca
                    break;
                }
                else {
                    init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));//vo value1 navratova hodnota
                    if(init != true) {
                        return -1;
                    }
                    inter_stack.top->next->eax=(*value1);
                    inter_stack_pop();//zrusenie aktualneho ramca
                    break;
                }

            case movr:
            case i_d_r:

            case jmp:
                I=(instr_t)I->dst;
                break;
            case jmpif:
            case jmpifn:
            case label:
                break;

            case int_to_dbl:
            case int_to_str:
            case bool_to_str:
            case dbl_to_str:


            case add:
            case sub:
            case imul:
                   init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                    if(init!=true) {
                        return -1;
                    }
                switch(dtype)
                    {
                        case dt_double:
                            src1_value=(*value1).d_val;
                            break;
                        case dt_int:
                            src1_value=(*value1).i_val;
                            break;
                    }
                init=decode_address(I->src2,&(value2),&(dtype),&(init_src2));
                    if(init!=true) {
                        return -1;
                    }
                switch(dtype)
                    {
                        case dt_double:
                            src2_value=(*value2).d_val;
                            break;
                        case dt_int:
                            src2_value=(*value2).i_val;
                            break;
                    }
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                switch(dtype)
                    {
                        case dt_double:
                            (*dest).d_val=src1_value*src2_value;
                            break;
                        case dt_int:
                            (*dest).i_val=src1_value*src2_value;
                            break;
                    }
                (*init_dest)=true;
                break;
            case idiv:
            case conc:
            case eql:
            case neq:
            case gre:
            case less:
            case geq:
            case leq:

            case or:
            case and:
            case not:

            case mov:
                break;
            case r_str:
            case r_int:
            case r_dbl:
            case len:
            case subs:
            case cmp:
            case findstr:
            case sortstr:
                break;
            case prnt:
                ifj16_print((inter_stack.top->vars)[0].val.s_val);
                break;

        }

        I=I->next;//next instruction
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
        *dtype = glob->dtype;
        *target = &(glob->val);
        *initialized = &(glob->initialized);
        return glob->initialized;
    } else if(op->sc == helper) {
        hlpr = (glob_helper_var_t)op;
        *dtype = hlpr->dtype;
        *target = &(hlpr->val);
        *initialized = &(hlpr->initialized);
        return hlpr->initialized; // this ain't just literal anymore ...
    } else if(op->sc == local) {
        loc = (local_var_t)op;
        *dtype = loc->dtype;
        *target = &(((call_stack_top->vars)[loc->index]).val);
        *initialized = &(((call_stack_top->vars)[loc->index]).initialized);
        return ((call_stack_top->vars)[loc->index]).initialized;
    }
    return false;
}

void inter_stack_init(){
    inter_stack.top=NULL;
}
//kontrolovat ci neni NULL pri volani
/*stackframe_t inter_stack_top(){
    stackframe_t top;
    top=inter_stack.top;
    return top;
}*/

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