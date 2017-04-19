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
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>

int queue_init(queue_t* queue){
	syslog(LOG_INFO, "Initializing data_queue");

	syslog(LOG_DEBUG, "Allocating head node");
	queue->head = malloc(sizeof(node_t));
	
	if (queue->head == NULL){
		syslog(LOG_ERR, "Failed to allocate head node!");
		return ENOMEM;
	}

	syslog(LOG_DEBUG, "Setting pointers");
	queue->head->data = NULL;
	queue->head->next = NULL;
	queue->tail = queue->head;
	queue->length = 0;

	syslog(LOG_DEBUG, "Initializing mutex");
	queue->queue_mutex = malloc(sizeof(pthread_mutex_t));
	int retval = pthread_mutex_init(queue->queue_mutex, NULL);
	syslog(LOG_DEBUG, "returned %d", retval);
	return retval;
}

int queue_destroy(queue_t* queue){
	while(queue->head->next != NULL){
		queue_pop(queue);
	}
	free(queue->head);
	int retval = pthread_mutex_destroy(queue->queue_mutex);
	free(queue->queue_mutex);
	return retval;
}

void queue_push(queue_t* queue, void* ptr){
	node_t* newnode = malloc(sizeof(node_t));
	newnode->data = ptr;
	newnode->next = NULL;
	int retval = pthread_mutex_lock(queue->queue_mutex);
	if (retval != 0){
		switch(retval){
			case EINVAL:
			case EAGAIN:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to invalid mutex!\n");
				break;
			case EDEADLK:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to deadlock condition!\n");
				break;
			default:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex, unknown error!\n");
				break;
		}
		return;
	}
	queue->tail->next = newnode;
	queue->tail = newnode;
	queue->length++;
	retval = pthread_mutex_unlock(queue->queue_mutex);
	if(retval != 0){
		switch(retval){
			case EINVAL:
			case EAGAIN:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to invalid mutex!\n");
				break;
			case EPERM:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex because wrong ownership!\n");
				break;
			default:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex, unknown error!\n");
				break;
		}
		return;
	}
}

void* queue_pop(queue_t* queue){
	if(!(queue->length)){
		return NULL;
	}
	node_t* retnode = (queue->head)->next;
	void* retval = retnode->data;
	int retval2 = pthread_mutex_lock(queue->queue_mutex);
	if (retval2 != 0){
		switch(retval2){
			case EINVAL:
			case EAGAIN:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to invalid mutex!\n");
				break;
			case EDEADLK:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to deadlock condition!\n");
				break;
			default:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex, unknown error!\n");
				break;
		}
		return NULL;
	}
	(queue->head)->next = retnode->next;
	queue->length--;
	if(retnode == queue->tail){
		queue->tail = queue->head;
	}
	retval2 = pthread_mutex_unlock(queue->queue_mutex);
	if(retval2 != 0){
		switch(retval2){
			case EINVAL:
			case EAGAIN:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex due to invalid mutex!\n");
				break;
			case EPERM:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex because wrong ownership!\n");
				break;
			default:
				fprintf(stderr, "queue.c: Failed to acquire queue mutex, unknown error!\n");
				break;
		}
		return NULL;
	}
	free(retnode);
	return retval;
}

int queue_isEmpty(queue_t* queue){
	return !(queue->length);
}