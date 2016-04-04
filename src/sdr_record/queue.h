#ifndef __QUEUE_H
#define __QUEUE_H

typedef struct queue queue;
typedef struct node node;
struct queue{
	node* head;
	node* tail;
	int length;
};

struct node{
	void* data;
	node* next;
};

void queue_init(queue* queue);
void queue_push(queue* queue, void* ptr);
void* queue_pop(queue* queue);
int queue_isEmpty(queue* queue);

#endif
