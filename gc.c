#include <stdlib.h>
#include "gc.h"


void *gc_malloc(size_t s){

	void *ptr;

	ptr = malloc(s);

	
	gc_push(hash(ptr), ptr);	

	return ptr;

}

void *gc_realloc(void *old, size_t s){
	void *new;

	new = realloc(old, s);

	if(new != old){
		gc_remove(hash(old), old);
		gc_push(hash(new), new);
	}

	return new;
}

void gc_push(int i, void *ptr){
	s_item tmp = malloc(sizeof(struct s_item));
	tmp->next = ht[i];
	tmp->data = ptr;
	ht[i] = tmp;
}

void gc_remove(int i, void *ptr){
	s_item tmp = ht[i];
	if(tmp){
		s_item backup;
		if(tmp->data == ptr){
			backup = tmp;
			tmp = tmp->next;
			free(backup);
			if(tmp == NULL)
				ht[i] = NULL;
		}
		else{
			while(tmp->next){
				if(tmp->next->data == ptr)
					break;
				tmp = tmp->next;
			}
			if(tmp->next){
				backup = tmp->next;
				tmp->next = tmp->next->next;
				free(backup);
			}
		}
	}
}

void free_all(){
	s_item tmp, backup;
	for(int i = 0; i < 769; i++){
		tmp = ht[i];
		while(tmp){
			backup = tmp->next;
			free(tmp->data);
			free(tmp);
			tmp = backup;
		}
		ht[i] = NULL;
	}
}
