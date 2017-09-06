/*
sandpile.c : Alex Giokas
Iowa State University - Com S 327 - Homework 0

Command line implementation of an abelian sandpile model
takes an arbitrary number of 3-tuples as arguments 
which correspond to (x, y) co-ordinates followed by a pile height

prints game states until the entire world is stable
*/

#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <unistd.h> // for usleep()
#include "queue.h"

#define clear() printf("\033[H\033[J")
const int width = 23;     
const int height = 23;
int world[23][23];

struct Queue *unstable; 

/*
Function: initalize_world
-------------------------
reads the list of input arguments and organizes them 
into 3-tuples which are then used to salt the world
*/

void initialize_world(int argc, char *argv[])
{
    printf("initializing world with %d arguments...\n", argc);
    //arg iterator variable
    int argnum;
    
    //parts of a 3-part positional argument
    //x, y = horizontal position in the world
    //h = height of a sandpile at position x, y
    int x, y, h;

    //assumes that the inputs are well formed
    //so each x and y argument has an integer value between 0 and 22 (inclusive)
    //and each h argument has a value between 0 and 4 (inclusive)
    for (argnum = 1; argnum < argc; argnum++) 
    {
        //which part of a tuple each arg represents
        //x is part 0, y is part 1, and h is part 2
        int part = (argnum - 1) % 3;

        printf(argv[argnum]);

        if (part == 0)
        {
            x = atoi(argv[argnum]);
        } 
        else if (part == 1) 
        {
            y = atoi(argv[argnum]);
        }
        else if (part == 2)
        {
            h = atoi(argv[argnum]);

            world[x][y] = h;
        }
    }
    printf("\n");
    unstable = init_queue(2*width*height); //queue capacity equal to twice total number of world cells
    //enqueue(unstable, -1); //-1 is the "end of turn" marker for the queue 
}

void add_sand()
{
    printf("adding sand...\n");
    world[(width/2)][(height/2)]++;

    if (world[(width/2)][(height/2)] == 5) {
        enqueue(unstable, (width/2));
        enqueue(unstable, (height/2));
    }
}

void topple_pile(int x, int y)
{
    world[x][y] -= 4;
    if (world[x][y] > 5) {
        enqueue(unstable, x);
        enqueue(unstable, y);
    }
    if (x > 0) { //pile is not on the left edge
        if (++world[x-1][y] == 5) { //iterate and check pile stability
            enqueue(unstable, x-1);
            enqueue(unstable, y);
        }
    }
    if (y > 0) { //pile is not on the top edge
        if (++world[x][y-1] == 5) {
            enqueue(unstable, x);
            enqueue(unstable, y-1);
        }
    }
    if (x < width-1) { //pile is not on the right edge
        if (++world[x+1][y] == 5) {
            enqueue(unstable, x+1);
            enqueue(unstable, y);
        }
    }
    if (x < width-1) { //pile is not on the bottom edge
        if (++world[x][y+1] == 5) {
            enqueue(unstable, x);
            enqueue(unstable, y+1);
        }
    }
}

    
    
void update_world()
{
    printf("updating world...\n");
    int x, y;
    x = peek(unstable);
    
    while (unstable->size > 1) { 
        x = dequeue(unstable); 
        
        y = dequeue(unstable);
        printf("y = %d  ", y);
        printf("queue head: %d\n", unstable->head);
        topple_pile(x, y);
    } 
}

            
void print_world(int iter)
{
    //row and column iterators
    printf("Abelian Sandpile, Iteration: %d \n", iter);
    printf("---------------------------------------------\n");
    int row, col;
    for (row = 0; row < width; row++)
    {
        for (col = 0; col < height; col++)
        {
            printf("%2d", world[row][col]);
            printf(" ");
        }
        printf("\n");
    }
}

void debug_print_unstable() 
{
    int i;
    struct Queue *temp = init_queue(unstable->capacity);
    int qsize = unstable->size;
    printf("Size: %d, Head: %d, Tail: %d\n", qsize, unstable->head, unstable->tail);
    printf("Unstable Piles: ");

    for (i = 0; i < qsize; i++) {
        if (i % 2 == 0) {
            int x = dequeue(unstable);
            printf("(%d, ", x);
            enqueue(temp, x);
        }
        else {
            int y = dequeue(unstable);
            printf("%d) ", y);
            enqueue(temp, y);
        }
    }
    printf("\n");
    
    int tsize = temp->size;
    for (i = 0; i < tsize; i++) {
        enqueue(unstable, dequeue(temp));
    }
}

int main(int argc,  char *argv[])
{
    initialize_world(argc, argv);
    print_world(0);

    int numiters = 10000;
    int i = 0;
    while (i < numiters) {
        add_sand(); 
        update_world();
        //clear();
        print_world(i+1);
        //debug_print_unstable();
        i++;
        usleep(1500000); //delay 0.1 second
    }

    return 0;
}
