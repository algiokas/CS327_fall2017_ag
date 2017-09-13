/*  pqueue.h : Alex Giokas
    Basic, General priority queue implemented on a heap
*/
#ifndef PQUEUE_H 

#define PQUEUE_H 

struct PQNode {
    int priority;
    void *data;
};


struct PQueue {
    struct PQNode *nodes;
    int elementSize;
    int capacity;
    int size;

};

int init_PQ(struct PQueue *newPQueue, int capacity, int elementsize);
int delete_PQ(struct PQueue *toDelete);
int is_empty(struct PQueue *pq);
int insert(struct PQueue *pq, int prio, void *data);
int remove_min(struct PQueue *pq, void *data);
int print_PQ(struct PQueue *pq);

#endif
