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

int edit_cell(struct Floor *floor, int x_loc, int y_loc, enum CellType input_type, int input_hardn);

int get_cell(struct Floor *floor, struct Cell *cellptr, int x_loc, int y_loc);

int place_room(struct Floor *floor, struct Room *room);

int check_intersection(struct Floor *floor, struct Room * room);

int add_rooms(struct Floor *floor);

int add_corridors(struct Floor *floor);

int init_floor(struct Floor *newFloor);

int delete_floor(struct Floor *floor);

void print_floor(struct Floor *floor);

#endif
