#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pqueue.h"

#define LEFT_CHILD(x) 2 * x + 1
#define RIGHT_CHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

int min_prio(struct PQueue *pq, int idxA, int idxB)
{
    if (pq->nodes[idxA].priority < pq->nodes[idxB].priority) {
        return idxA;
    }
    return idxB;
}

int init_PQ(struct PQueue *newPQ, int capacity, int elementsize)
{
    printf("Initializing Priority Queue... \n");
    newPQ->nodes = (struct PQNode *) malloc(capacity * sizeof(struct PQNode));
    newPQ->elementSize = elementsize;
    newPQ->capacity = capacity;
    newPQ->size = 0;
    return 0;
}

int delete_PQ(struct PQueue *toDelete)
{
    printf("Deleting Priority Queue... \n");
    if (!toDelete) {
        return 1;
    }
    //need a temporary void pointer as an argument for remove_min
    void *temp = malloc(toDelete->elementSize);
    while (!remove_min(toDelete, temp))
        ;
    free(toDelete->nodes);
    free(toDelete);
    free(temp);
    return 0;
}

int percolate_up(struct PQueue *pq, int nodeIndex) {
    printf("Percolating Up... \n");
    int idx = nodeIndex;
    if (pq->nodes[idx].priority < pq->nodes[PARENT(idx)].priority) {
        struct PQNode temp = pq->nodes[idx];
        pq->nodes[idx] = pq->nodes[PARENT(idx)];
        pq->nodes[PARENT(idx)] = temp;
        
        //Changed to recursion here to maintain consistency with percolate_down()
        percolate_up(pq, PARENT(idx));
    }
    return 0;
}

int percolate_down(struct PQueue *pq, int nodeIndex) {
    printf("Percolating Down... \n");
    if (nodeIndex+1 >= pq->size) {
        return 0;
    }
    int idx = nodeIndex;
    int left = LEFT_CHILD(idx);
    int right = RIGHT_CHILD(idx);
    if (right >= pq->size) {
        right = left;
    }
    
    if (pq->nodes[idx].priority > pq->nodes[left].priority
        || pq->nodes[idx].priority > pq->nodes[right].priority)
        {
        int minChild = min_prio(pq, left, right);
        struct PQNode temp = pq->nodes[idx];
        pq->nodes[idx] = pq->nodes[minChild];
        pq->nodes[minChild] = temp;
       
        percolate_down(pq, minChild);
    }
    return 0;
}

int is_empty(struct PQueue *pq)
{
    return (!pq->size);
} 


int insert(struct PQueue *pq, int prio, void *data)
{
    printf("Inserting Element with Priority %d ... \n", prio);
    //if the pq reaches capacity, double its size
    if (pq->size + 1 > pq->capacity) {
        pq->capacity = pq->capacity * 2;
        pq->nodes = realloc(pq->nodes, pq->capacity * sizeof(struct PQNode));
    }

    //allocate space for the data stored in the new node
    void *dataptr = malloc(pq->elementSize);
    struct PQNode newNode;
    newNode.data = dataptr;

    //copy the data from the void *data argument into the newly allocated pointer
    memcpy(newNode.data, data, pq->elementSize);
    int end = pq->size++;
    newNode.priority = prio;
    pq->nodes[end] = newNode;

    if (!percolate_up(pq, end)) {
        return 1;
    }
    return 0;
}

int remove_min(struct PQueue *pq, void *data)
{
    printf("Removing Min Element... \n");
    //printf("removing min...\n");
    if(!pq->size) {
        return 1;
    }
    memcpy(data, pq->nodes[0].data, pq->elementSize);
    free(pq->nodes[0].data);
    if (pq->size > 1) {
        //move last node to the root of the heap
        pq->nodes[0] = pq->nodes[pq->size - 1];
        percolate_down(pq, 0);
    }
    pq->size--;
    return 0;
}

int print_PQ(struct PQueue *pq) {
    printf("Priorty Queue ---\n");
    printf("Element Size: %d \n", pq->elementSize);
    printf("Queue Capacity: %d \n", pq->capacity);
    printf("Queue Size: %d \n", pq->size);
    int i;
    printf("Queue Contents (Priority): {");
    for (i = 0; i < pq->size; i++) {
        printf("%d", pq->nodes[i].priority);
        if (i < pq->size - 1) {
            printf(", ");
        }
    }
    printf("}\n\n");
}


    
