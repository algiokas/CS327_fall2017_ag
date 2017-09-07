#ifndef DUNGEON_H

#define DUNGEON_H


//A simple struct to store two integers together
struct Duo {
    int x;
    int y;
};

//enumerates possible cell types
enum CellType {
    immutable_c,
    rock_c,
    room_c,
    corridor_c
};

//a cell in the dungeon. Cells have a type and a hardness, they may also have additional 
//properties in the future. hardness is a property of all cells, however only rock has 
//variable hardness, the hardness of open cells (room,corridor) is 0 and the hardness of immutable
//cells is 11
struct Cell {
    enum CellType type;
    int hardness;
};

//encapsulates a room, loc is the location of the top-left corner of the room
//dims describes the dimensions (width, height)
struct Room {
    struct Duo loc;
    struct Duo dims;
};

//A floor is an 80x21 map which is a single layer of the dungeon
struct Floor {
    int width;
    int height;
    int numRooms;
    struct Room *rooms;
    struct Cell *map; //this will be a 2D array but we want memory allocated in a contiguous block
};

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
int edit_cell(struct Floor *floor, int x_loc, int y_loc, enum CellType input_type, int input_hardn);


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
int get_cell(struct Floor *floor, struct Cell *cellptr, int x_loc, int y_loc);

/*  Function: place_room
    --------------------
    Takes a floor pointer and a room pointer and modifies the map of the floor
    so that the area of the room contains "room" cells

    floor: pointer to the floor being modified
    room: pointer to the room to be placed in floor
*/
int place_room(struct Floor *floor, struct Room *room);

/*  Function: check_intersection
    ----------------------------
    Takes a floor pointer and a room pointer and determines whether or not the room
    intersects any of the existing rooms already on that floor

    floor: Pointer to the floor containing rooms that the new room needs to avoid
    room: Pointer to a new room being checked

    returns: 0 if no intersection is detected, 1 if there is an intersection
*/
int check_intersection(struct Floor *floor, struct Room * room);

/*  Function: add_rooms
    -------------------
    randomly generates rooms and adds them to a floor both by modifying the floor's map
    and by adding them to the floor's rooms list

    floor: pointer to the floor being modified
 */
int add_rooms(struct Floor *floor);


/*  Function: add_corridors
    -----------------------
    Uses dijkstra's algorithm to add corridors between each consecutive room
    in a floor's dynamic rooms array

    floor: pointer to the floor being modified
*/
int add_corridors(struct Floor *floor);

/*  Function: init_floor
    --------------------
    Initalizes a floor pointer by setting starting values and allocating memory for
    pointer members

    returns: 0 if successful
*/
int init_floor(struct Floor *newFloor);

/*  Function: delete_floor
    ----------------------
    deletes a floor by freeing the memory for its fields then freeing the memory pointer
    itself

    returns: 0 if successful
*/
int delete_floor(struct Floor *floor);

/*  Function: print_floor
    ---------------------
    prints the map of a floor to the console with a border

    floor: a pointer to the floor to be printed
*/
void print_floor(struct Floor *floor);

#endif
