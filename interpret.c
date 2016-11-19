#include "interpret.h"
#include "sym_table.h"
#include "ilist.h"
#include "ifj16_class.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// src1_value and src2_value ARE INTEGERS, DOUBLES ARE NOT SUPPORTED!

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
    char arr[50];

    while(I) { 
        
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
                
                if(I->src1==NULL) { //void, do eax sa nedava nic
                    I=inter_stack.top->ret_addr;//pokracovat odkial sa skocilo
                    inter_stack_pop();//zrusenie aktualneho ramca
                    continue;
                }
                else {
                    init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));//vo value1 navratova hodnota
                    if(init != true) {
                        return -1;
                    }
                    inter_stack.top->next->eax=(*value1);
                    I=inter_stack.top->ret_addr;//pokracovat odkial sa skocilo
                    inter_stack_pop();//zrusenie aktualneho ramca
                    continue;
                }

            case movr:
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                *dest = (inter_stack.top)->eax;
                *init_dest = true;
                break;
            case i_d_r:

            case jmp:
                I=(instr_t)I->dst;
                break;
            
            case jmpif:
            case jmpifn:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                if(!(value1->b_val)){
                    I=(instr_t)I->dst;
                }
                break;
            case label:
                break;

            case int_to_dbl:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                if(init != true){
                        return -1;
                }
                (*init_dest)=true;
                (*dest).d_val=(double)(*value1).i_val;
                break;                
            
            case int_to_str:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                sprintf(arr, "%d", value1->i_val);
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                (*init_dest)=true;
                dest->s_val = str_init(arr);
                break;
            
            case bool_to_str:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                if(init != true){
                        return -1;
                }
                (*init_dest)=true;
                if((*value1).b_val) {
                    sprintf(arr, "%s", "true");
                }
                else {
                    sprintf(arr, "%s", "false");
                }
                dest->s_val = str_init(arr);
            
            case dbl_to_str:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                sprintf(arr, "%g", value1->d_val);
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                if(init != true){
                        return -1;
                }
                (*init_dest)=true;
                dest->s_val = str_init(arr);
                break;
            
            case add:
            
            case sub:
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
                            (*dest).d_val=src1_value-src2_value;
                            break;
                        case dt_int:
                            (*dest).i_val=src1_value-src2_value;
                            break;
                    }
                (*init_dest)=true;
                break;

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
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1)); // need to check if initialized
                init=decode_address(I->src2,&(value2),&(dtype),&(init_src2)); // need to check if initialized
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                dest->s_val = str_init(value1->s_val->data);
                str_cat(dest->s_val, value2->s_val);
                *init_dest = true;
                break;
            
            case eql:
                break;
            case neq:
            case gre:
            case less:
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
                dest->b_val = src1_value < src2_value;
                (*init_dest)=true;
                break;
            case geq:
            case leq:

            case or:
            case and:
            case not:

            case mov:
                init=decode_address(I->src1,&(value1),&(dtype),&(init_src1));
                if(init != true){
                        return -1;
                }
                init=decode_address(I->dst,&(dest),&(dtype),&(init_dest));
                *dest=*value1;
                *init_dest=true;
                break;

            case r_str://not implemeted yet
                (inter_stack.top)->vars[0].val.s_val=ifj16_readString();
                (inter_stack.top)->vars[0].initialized = true;
                break;
            
            case r_int://not implemeted yet
                (inter_stack.top)->vars[0].val.i_val=ifj16_readInt();
                (inter_stack.top)->vars[0].initialized = true;
                break;
            
            case r_dbl://not implemeted yet
                (inter_stack.top)->vars[0].val.d_val=ifj16_readDouble();
                (inter_stack.top)->vars[0].initialized = true;
                break;
            
            case len:
                (inter_stack.top)->vars[1].val.i_val=ifj16_length((inter_stack.top)->vars[0].val.s_val);
                (inter_stack.top)->vars[1].initialized = true;
                break;
            
            case subs://not implemeted yet
                (inter_stack.top)->vars[3].val.s_val=ifj16_substr((inter_stack.top)->vars[0].val.s_val,(inter_stack.top)->vars[1].val.i_val,(inter_stack.top)->vars[2].val.i_val);
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
                (inter_stack.top)->vars[1].initialized = true;
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
        *target = &(((inter_stack.top->vars)[loc->index]).val);
        *initialized = &(((inter_stack.top->vars)[loc->index]).initialized);
        return ((inter_stack.top->vars)[loc->index]).initialized;
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