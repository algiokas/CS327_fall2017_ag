#include <stdio.h>

#include "queue.h"

int main (int argc, char *argv[])
{
    struct Queue *queue = init_queue(10);

    enqueue(queue, 1);
    enqueue(queue, 2);
    enqueue(queue, 3);
    enqueue(queue, 4);
    enqueue(queue, 5);

    printf("%d \n", dequeue(queue));
    printf("%d \n", dequeue(queue));
    printf("%d \n", dequeue(queue));
    printf("%d \n", dequeue(queue));
    printf("%d \n", dequeue(queue));
    return 0;
}

