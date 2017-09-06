#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

struct Queue* init_queue(int capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->head = 0;
    queue->tail = 0; 
    queue->size = 0;
    queue->capacity = capacity;

    queue->data = (int*) malloc(capacity * sizeof(int));
}

//checks whether a queue is full
int is_full(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int is_empty(struct Queue* queue)
{
    return (queue->size == 0);
}

void enqueue(struct Queue* queue,  int item)
{
    if (is_full(queue)) {
        return;
    }
    queue->data[queue->tail] = item;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
}

int dequeue(struct Queue* queue)
{
    if (is_empty(queue)) {
        return 0;
    }
    
    int item = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    return item;
}

int peek(struct Queue* queue)
{
    if (is_empty(queue)) {
        return 0;
    }
    return queue->data[queue->head];
}


