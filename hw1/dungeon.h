#ifndef DUNGEON_H

#define DUNGEON_H


//A simple struct to store two integers together
typedef struct Duo {
    int x;
    int y;
} Duo;

//enumerates possible cell types
enum type CellType {
    immutable;
    rock;
    room;
    corridor;
};

//a cell in the dungeon. Cells have a type and a hardness, they may also have additional 
//properties in the future. hardness is a property of all cells, however only rock has 
//variable hardness, the hardness of open cells (room,corridor) is 0 and the hardness of immutable
//cells is 11
typedef struct Cell {
    CType type;
    int hardness;
};

//encapsulates a room, loc is the location of the top-left corner of the room
//dims describes the dimensions (width, height)
typedef struct Room {
    Duo loc;
    Duo dims;
};

//A floor is an 80x21 map which is a single layer of the dungeon
typedef struct Floor {
    int width;
    int height;
    int numRooms;
    Room *rooms;
    Cell *map; //this will be a 2D array but we want memory allocated in a contiguous block
};



/*  Function: add_rooms
    -------------------
    randomly generates rooms and adds them to a floor both by modifying the floor's map
    and by adding them to the floor's rooms list

    floor: pointer to the floor being modified
 */
int add_rooms(Floor *floor);


/*  Function: generate_floor
    -----------------------
    Generate a floor by first creating an empty floor, then populating it with rooms
    using add_rooms and finally adding corridors using add_corridors

    returns: a pointer to the newly generated floor
*/
Floor* generate_Floor();

/*  Function: add_corridors
    -----------------------
    Uses dijkstra's algorithm to add corridors between each consecutive room
    in a floor's dynamic rooms array

    floor: pointer to the floor being modified
*/
int add_corridors(Floor *floor);


/*  Function: print_floor
    ---------------------
    prints the map of a floor to the console with a border

    floor: a pointer to the floor to be printed
*/
void print_floor(Floor *floor);

#endif
