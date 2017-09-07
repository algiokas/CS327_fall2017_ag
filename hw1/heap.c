#include <stdlib.h>

#include "heap.h"

#define LEFT_CHILD(x) 2 * x + 1
#define RIGHT_CHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

int init_heap(struct Heap *newHeap, int capacity)
{
    newHeap->nodes = (struct HeapNode *) malloc(capacity * sizeof(struct HeapNode));
    newHeap->capacity = capacity;
    newHeap->size = 0;
    return 0;
}

int delete_heap(struct Heap *toDelete)
{
    if (!toDelete) {
        return 1;
    }
    free(toDelete->nodes);
    free(toDelete);
    return 0;
}

int insert(struct Heap *heap, int prio, int x_data, int y_data) 
{
    //if the heap reaches capacity, double its size
    if (heap->size + 1 > heap->capacity) {
        heap->size = heap->size * 2;
        heap->nodes = realloc(heap->nodes, heap->size * sizeof(struct HeapNode)); 
    }
    
    int end = heap->size++;
    struct HeapNode newNode;
    newNode.priority = prio;
    newNode.x = x_data;
    newNode.y = y_data;
    heap->nodes[end] = newNode;

    
    

    return 0;
}

int remove_min(struct Heap *heap, void *data)
{
    //TODO
    return 0;
}
