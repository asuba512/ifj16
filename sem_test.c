#include "scanner.h"
#include "parser.h"
#include "token.h"
#include "infinite_string.h"
#include "sym_table.h"
#include "ial.h"
#include <stdio.h>
#include <stdlib.h>
#include "sem_test.h"


char *types[5] = {"double", "int", "String", "boolean", "void"};

extern string_t buff; // <- variable which has to be destroyed before exit, internal scanner variable
extern token_t t;
extern FILE *fd;
extern int lexerror;

// copypaste
int main(int argc, char **argv){
	(void)argc;
	fd = fopen(argv[1],"r");
	if(fd == NULL){
		printf("error - please provide some source code to play with\n");
		return 99;
	}
	init_class_table();

	t = malloc(sizeof(struct token));

	int retval = c_list();
	
	show_hierarchy();
	int c;
	while((c = getc(fd)) != EOF){
		putchar(c);
	}
	
	str_destroy(buff);

	free(t);
	fclose(fd);
    return 0;
}

void show_hierarchy() {
    printf("\n---------- Symbols table dump: ------------ \n");
    bst_inorder(classes->root, print_class);
}

void print_class(void *class) {
    class_t c = (class_t)class;
    printf("Class \"%s\":\n", c->id->data);
    bst_inorder(c->root, print_member);
}

void print_member(void *memb) {
    class_memb_t m = (class_memb_t)memb;
    char *memb_type = m->type == var ? "Variable" : "Function";
    if(m->type == var) {
        printf("\t%s: %s %s\n", memb_type, types[m->dtype], m->id->data);
    }
    if(m->type == func) {
        printf("\t%s: %s %s(", memb_type, types[m->dtype], m->id->data);
        for(int i = 0; i < (m->arg_count); i++) {
            if(i == m->arg_count - 1) {
                printf("%s %s", types[(m->arg_list)[i]->dtype], (m->arg_list)[i]->id->data);
            } else {
                printf("%s %s, ", types[(m->arg_list)[i]->dtype], (m->arg_list)[i]->id->data);
            }
        }
        printf(");\n");
        printf("\t\tVariables in tree (Arg count: %d):\n", m->arg_count);
        bst_inorder(m->local_sym_table_root, print_loc_var);
    }
}

void print_loc_var(void *var) {
    local_var_t v = (local_var_t)var;
    printf("\t\t\t%d: %s %s\n", v->index, types[v->dtype], v->id->data);
}
