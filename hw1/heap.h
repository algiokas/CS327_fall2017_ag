
#ifndef HEAP_H

#define HEAP_H

#include "dungeon.h"

//heap is temporarily designed to store a pair of 2D co-ordinates for pathfinding
//I plan to make this a more general use heap
struct HeapNode {
    int priority;
    int x;
    int y;
};


struct Heap {
    struct HeapNode *nodes;
    int capacity;
    int size;
};

int init_heap(struct Heap *newHeap, int capacity);
int delete_heap(struct Heap *toDelete);
int insert(struct Heap *heap, int prio, int x, int y); 
int remove_min(struct Heap *heap, struct Duo* minData);


