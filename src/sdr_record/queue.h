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
#ifndef __QUEUE_H
#define __QUEUE_H

#include <pthread.h>

typedef struct queue queue_t;
typedef struct node node_t;
struct queue{
	node_t* head;
	node_t* tail;
	int length;
	pthread_mutex_t* queue_mutex;
};

struct node{
	void* data;
	node_t* next;
};

int queue_init(queue_t* queue);
void queue_push(queue_t* queue, void* ptr);
void* queue_pop(queue_t* queue);
int queue_isEmpty(queue_t* queue);
int queue_destroy(queue_t* queue);

#endif