// Includes
#include "queue.h"
#include <stdlib.h>

void queue_init(queue* queue){
	queue->head = malloc(sizeof(node));
	queue->head->data = NULL;
	queue->head->next = NULL;
	queue->tail = queue->head;
	queue->length = 0;
}

void queue_push(queue* queue, void* ptr){
	node* newnode = malloc(sizeof(node));
	newnode->data = ptr;
	newnode->next = NULL;
	queue->tail->next = newnode;
	queue->tail = newnode;
	queue->length++;
}

void* queue_pop(queue* queue){
	if(!(queue->length)){
		return NULL;
	}
	node* retnode = (queue->head)->next;
	(queue->head)->next = retnode->next;
	void* retval = retnode->data;
	queue->length--;
	if(retnode == queue->tail){
		queue->tail = queue->head;
	}
	free(retnode);
	return retval;
}

int queue_isEmpty(queue* queue){
	return !(queue->length);
}
