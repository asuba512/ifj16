#include "token.h"
#include <stdlib.h>

tok_que_t tok_que_init() {
    tok_que_t new_que = malloc(sizeof(struct tok_que));
    if(new_que == NULL)
        return NULL;
    new_que->head = NULL;
    new_que->tail = NULL;
    return new_que;
}

int tok_enqueue(tok_que_t queue, struct token t) {
    tok_que_node_t new_node = malloc(sizeof(struct tok_que_node));
    if(new_node == NULL)
        return 99;
    new_node->next = NULL;
    new_node->tok = t;
    if(queue->head == NULL)
        queue->head = new_node;
    else
        queue->tail->next = new_node;
    queue->tail = new_node;
    return 0;
}

token_t tok_remove_head(tok_que_t queue) {
	token_t token_tmp;
    if(queue->head != NULL) {
        tok_que_node_t tmp = queue->head;
	    queue->head = queue->head->next;
		token_tmp = tmp->tok;
        free(tmp);
    }
	return token_tmp;
}

void tok_que_destroy(tok_que_t queue) { // check for empty queue?
    tok_que_node_t prev = queue->head;
    for(tok_que_node_t item = prev->next; item != NULL; item = item->next) {
		free(prev);
		prev = item;
    }
    free(prev);
    queue->head = queue->tail = NULL;
}

int tok_que_empty(tok_que_t queue){
	return queue->head == NULL;
}
