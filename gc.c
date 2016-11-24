#include <stdlib.h>
#include "gc.h"


void *gc_malloc(size_t s){
	void *ptr;
	ptr = malloc(s);
	if(ptr){ // if enough memory, push pointer to hash table
		if(gc_push(hash(ptr), ptr)){ // if cannot push new pointer 
			free(ptr);
			return NULL; // this will cause return 99 - internal error somewhere else
		}
	}
	return ptr;

}

void *gc_realloc(void *old, size_t s){
	void *new;
	new = realloc(old, s);
	if(new != old){ // if data are located in new place in memory
		gc_remove(hash(old), old); // remove old pointer from hash table
		gc_push(hash(new), new); // add new pointer to hash table
	}
	return new;
}

int gc_push(int i, void *ptr){
	s_item tmp = malloc(sizeof(struct s_item));
	if(!tmp)
		return 99;
	tmp->next = ht[i];
	tmp->data = ptr;
	ht[i] = tmp;
	return 0;
}

void gc_remove(int i, void *ptr){
	s_item tmp = ht[i];
	if(tmp){
		s_item backup;
		if(tmp->data == ptr){ // if the very first element is searched pointer
			backup = tmp;
			ht[i] = tmp->next;
			free(backup);
		}
		else{
			while(tmp->next){ // until next pointer is NULL
				if(tmp->next->data == ptr) // if next pointer is searched one
					break;
				tmp = tmp->next;
			}
			if(tmp->next){ // if pointer was found
				backup = tmp->next;
				tmp->next = tmp->next->next;
				free(backup);
			}
		}
	}
}

void free_all(){
	s_item tmp, backup;
	for(int i = 0; i < 769; i++){ // for every row in hash table
		tmp = ht[i];
		while(tmp){ // and every element in current row
			backup = tmp->next;
			free(tmp->data);
			free(tmp);
			tmp = backup;
		}
		ht[i] = NULL;
	}
}
