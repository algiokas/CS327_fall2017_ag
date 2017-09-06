#ifndef QUEUE_H

#define QUEUE_H

struct Queue
{
    int head;
    int tail;
    int size; //current number of elements
    int capacity;
    int *data;
};

struct Queue* init_queue(int capacity);

int is_full(struct Queue* queue);

int is_empty(struct Queue* queue);

void enqueue(struct Queue* queue, int item);

int dequeue(struct Queue* queue);

int peek(struct Queue* queue);


#endif
