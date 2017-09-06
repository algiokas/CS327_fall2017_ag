/*  Dungeon.c : Alex Giokas
    Iowa State University - Com Sci 327 - Fall 2017
    Handles the dungeon state and functions that modify it */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dungeon.h"

#define FWIDTH  80 //floor width
#define FHEIGHT 21 //floor height

#define MINROOMS  5 //minimum rooms per floor as per assignment spec
#define MAXROOMS 15 //arbitrary upper bound
#define MINROOMWIDTH 3 //minimum width of a room
#define MINROOMHEIGHT 2 //minimum height of a room
#define ROOMDENSITY 0.07 //the fraction of the floor that must be rooms

/*  Function edit_cell
    ------------------
    Edits a particular cell at location (x_loc, y_loc) to be celltype type and have
    hardness hard. Because the "map" array that stores the cells on each floor is allocated
    as a 1D arrray, pointer arithmetic is necessary. This function serves to encapsulate that
    pointer arithmetic.

    floor: The floor on which a cell is to be modified
    x_loc, y_loc: x and y co-ordinates of the cell to be modified
    type: the new type value for the cell
    hard: the new hardness value for the cell

    returns: 0 if successful
*/
int edit_cell(struct Floor *floor, int x_loc, int y_loc, enum CType input_type, int input_hardn)
{
    int cellIndex = (FWIDTH * y_loc) + x_loc;
    floor->map[cellIndex]->type = input_type;
    floor->map[cellIndex]->type = input_hardn;
    return 0;
}

/*  Function get_cell
    -----------------
    Takes a cell pointer and sets its value to the address of the cell at
    location (x_loc, y_loc). Similar to edit_cell, this is mainly just an abstraction
    of pointer arithmetic to make the floor "map" member feel 2D.

    floor: The floor from which we are getting a cell
    cellptr: The pointer where we will assign our cell data 
    x_loc, y_loc: the x and y co-ordinates of the cell we are getting

    returns: 0 if successful
*/
int get_cell(struct Floor *floor, struct Cell *cellptr, int x_loc, int y_loc)
{
    int cellIndex = (FWIDTH * y_loc) + x_loc;
    cellptr->type = floor->map[cellIndex]->type;
    cellptr->hardness = floor->map[cellIndex]->type;
}
    
/*  Function: place_room
    --------------------
    Takes a floor pointer and a room pointer and modifies the map of the floor
    so that the area of the room contains "room" cells

    floor: pointer to the floor being modified
    room: pointer to the room to be placed in floor
*/
int place_room(struct Floor *floor, struct Room *room)
{
    printf("Placing Room...");
    int row, col;
    for (col = 0; col < floor->dims->y; col++) {
        for (row = 0; row < floor->dims->x; row++) {
            edit_cell(floor, row, col, room, 0);
        }
    }
    return 0;
}

/*  Function: check_intersection
    ----------------------------
    Takes a floor pointer and a room pointer and determines whether or not the room
    intersects any of the existing rooms already on that floor

    floor: Pointer to the floor containing rooms that the new room needs to avoid
    room: Pointer to a new room being checked

    returns: 0 if no intersection is detected, 1 if there is an intersection
*/
int check_intersection(struct Floor *floor, struct Room * room)
{
    printf("Checking Intersection...");
    if (!floor->numRooms) {
        return 0;
    }
    int roomIter;
    for (roomIter = 0; roomIter < floor->numRooms; roomIter++) {
        //temporary variables assigned for clarity
        int new_left = room->loc->x; 
        int new_right = room->loc->x + room->dims->x;
        int new_top = room->loc->y;
        int new_bottom = room->loc->y + room->dims->y;

        int old_left = floor->rooms[roomIter]->loc->x;
        int old_right = floor->rooms[roomIter]->loc->x + floor->rooms[roomIter]->dims->x;
        int old_top = floor->rooms[roomIter]->loc->y;
        int old_bottom = floor->rooms[roomIter]->loc->y floor->rooms[roomIter]->dims->y;
        
        int x_intersect = 0;
        int y_intersec = 0;

        if (old_left <= new_left && new_left <= old_right) { x_intersect = 1; }
        if (old_left <= new_right && new_right <= old_right) { x_intersect = 1; }
        if (old_top <= new_top && new_top <= old_bottom) { y_intersect = 1; }
        if (old_top <= new_bottom && new_bottom <= old_bottom) { y_intersect = 1; }

        if (x_intersect && y_intersect) { return 1; }
    }
    return 0;
}

/*  Function: add_rooms
    -------------------    
    randomly generates rooms and adds them to a floor both by modifying the floor's map
    and by adding them to the floor's rooms list 
    
    floor: pointer to the floor being modified
 */
int add_rooms(Floor *floor)
{
    printf("Adding Rooms...");
    double floorsize = floor->width * floor->height;
    double freespace = floorsize; //the total amount of free space within which to place rooms
    
    int roomIter = 0;
    struct Room *newRoom; //blank room that we can fill with data and pass to functions
    newRoom = (Room *) malloc(sizeof(Room));

    while ((floor->numRooms < MINROOMS || (freespace / floorspace) > 1 - ROOMDENSITY)
            && floor->numRooms < MAXROOMS) {
        //Rooms should never touch the edge
        newRoom->loc->x = 1 + (rand() % (FWIDTH - MINROOMWIDTH - 1));
        newRoom->loc->y = 1 + (rand() % (FHEIGHT - MINROOMHEIGHT - 1));
        //we don't want floors more than half the width or height of the dungeon
        newRoom->size->x = MINROOMWIDTH + (rand() % ((FWIDTH / 2) - MINROOMWIDTH));
        newRoom->size->y = MINROOMHEIGHT + (rand() % ((FHEIGHT / 2) - MINROOMHEIGHT));

        if (!check_intersection(floor, newRoom)) {
            floor->rooms[roomIter] = *newRoom;
            floor->numRooms++;
            place_room(floor, newRoom);
            //get the total space taken up by the new room
            int roomsize = newRoom->size->y + newRoom->size->x;
            freespace -= roomsize; //and subtract it from free space
            roomIter++;
        }
    }
    free(newRoom);
}


/*  Function: add_corridors
    -----------------------
    Uses dijkstra's algorithm to add corridors between each consecutive room
    in a floor's dynamic rooms array

    floor: pointer to the floor being modified
*/
int add_corridors(struct Floor *floor)
{
    printf("Adding Corridors...");
    //TODO
    return 0; 
}

/*  Function: init_floor 
    --------------------
    Initalizes a floor pointer by setting starting values and allocating memory for
    pointer members
    
    returns: 0 if successful
*/
int init_Floor(struct Floor *newFloor)
{
    newFloor->width = FWIDTH;
    newFloor->height = FHEIGHT;
    newFloor->numRooms = 0;

    newFloor->rooms = (Room *) malloc(MAXROOMS * sizeof(Room));
    newFloor->map = (Cell *) malloc(FHEIGHT * sizeof(Cell *));
    
    return 0;
}

/*  Function: delete_floor
    ----------------------
    deletes a floor by freeing the memory for its fields then freeing the memory pointer
    itself

    returns: 0 if successful
*/
int delete_floor(struct Floor *floor)
{
    free(floor->rooms);
    free(floor->map);
    free(floor);

    return 0;
}

/*  Function: print_floor
    ---------------------
    prints the map of a floor to the console with a border

    floor: a pointer to the floor to be printed
*/
void print_floor(struct Floor *floor)
{
    int row, col, dash;
    //top edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
    struct Cell *cell = (struct Cell*) malloc(sizeof(cell));
    for (col = 0; col < FHEIGHT; col++) {
        printf("|");
        for (row = 0; row < FWIDTH; row++) {
            get_cell(floor, cell, row, col);
            switch(cell->type) {
                case(immutable) :
                    printf(" ");
                    break;
                case(rock) :
                    printf(" ");
                    break;
                case(room) :
                    printf(" ");
                    break;
                case(corridor) :
                    printf(" ");
                    break;
                default :
                    printf("X"); //indicates an error
            }
        }
        printf("|\n");
    }
    free(cell);
    //bottom edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
}
 

int main(int argc, char *argv[])
{
    srand(time(NULL));
    struct Floor *newFloor = (Floor *) malloc(sizeof(Floor));
    generate_floor(newFloor);
    print_floor(newFloor);
    return 0;
}
