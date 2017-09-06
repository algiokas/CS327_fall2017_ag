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

int numiters = 1000;
/*
Function: initalize_world
-------------------------
reads the list of input arguments and organizes them 
into 3-tuples which are then used to salt the world
*/

void initialize_world(int argc, char *argv[])
{
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

//adds a single grain of sand to the center of the world and modifies the queue if this makes
//the center point unstable
void add_sand()
{
    //printf("adding sand...\n");
    world[(width/2)][(height/2)]++;

    if (world[(width/2)][(height/2)] == 5) {
        enqueue(unstable, (width/2));
        enqueue(unstable, (height/2));
    }
}

//takes the pile at position (x, y) and topples it according to the rules of the sandpile
void topple_pile(int x, int y)
{
    world[x][y] -= 4;
    if (world[x][y] > 5) {
        enqueue(unstable, x);
        enqueue(unstable, y);
    }
    /* Student Note: Since sand on a pile is added one grain at a time everywhere except the
    initial salt, checking when the pile had a height of exactly 5 was my solution to make sure
    that no pile was added to the unstable queue twice. I'm not sure if this is exactly the best
    way to prevent such an issue, but it seems to work here as well as when I use it in add_sand. */
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


//prints the world to console, takes an integer argument "iter" which is printed at the top
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

//runs through the queue of unstable piles and topples them until 
void update_world()
{
    //printf("updating world...\n");
    int x, y;
    while (unstable->size > 1) { 
        x = dequeue(unstable); 
        y = dequeue(unstable);
        topple_pile(x, y);
    } 
}

/*
//For debugging, prints the size, head index, tail index and contents of the unstable queue
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
*/

int main(int argc,  char *argv[])
{
    int iter = 0;
    clear();    
    initialize_world(argc, argv);
    print_world(0);
    usleep(1000000);
    while (iter < numiters) {
        add_sand(); 
        update_world();
        clear();  
        print_world(iter+1);
        iter++;
        usleep(100000); //delay 0.1 second
    } 
    return 0;
}
