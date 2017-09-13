#include <stdlib.h>
#include <stdio.h>

#include "pqueue.h"
#include "dungeon.h"

int main(int argc, char *argv[]) {
    struct PQueue *pq = (struct PQueue *) malloc(sizeof(struct PQueue));
    init_PQ(pq, 10, sizeof(sizeof(struct Duo)));
    struct Duo *data1 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data2 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data3 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data4 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data5 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data6 = (struct Duo *) malloc(sizeof(struct Duo));
    struct Duo *data7 = (struct Duo *) malloc(sizeof(struct Duo));

    data1->x = 5;
    data1->y = 8;

    data2->x = 2;
    data2->y = 6;
    
    data3->x = 4;
    data3->y = 1;
   
    data4->x = 3;
    data4->y = 1;
    
    data5->x = 5;
    data5->y = 9;
   
    data6->x = 5;
    data6->y = 3;
    
    data7->x = 9;
    data7->y = 8;
    
    
    insert(pq, 6, data1);
    insert(pq, 4, data2);
    insert(pq, 2, data3);
    insert(pq, 1, data4);
    insert(pq, 3, data5);
    insert(pq, 5, data6);
    insert(pq, 7, data7);
    
    printf("data1: (%d, %d)\n", data1->x, data1->y);

    struct Duo *temp = (struct Duo *) malloc(2 * sizeof(struct Duo));
    
    print_PQ(pq);
    printf("Queue Values (expected 3 1 4 1 5 9 2 6 5 3 5 8 9 8): ");
    while(!is_empty(pq)) {

        remove_min(pq, temp);
        printf("(%d, %d) ", temp->x, temp->y);
    }
    printf("\n");
    free(data1);
    free(data2);
    free(data3);
    free(data4);
    free(data5);
    free(data6);
    free(data7);
    free(temp);
    free(pq);
    return 0;
}
