/*
RCT Payload Software
Copyright (C) 2016  Hui, Nathan T.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
