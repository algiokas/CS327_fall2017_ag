#ifndef DUNGEON_H

#define DUNGEON_H


//A simple struct to store two integers together
struct Duo {
    int x;
    int y;
};

//enumerates possible cell types
typedef enum cellType {
    immutable_c,
    rock_c,
    room_c,
    corridor_c,
    error_c
} CType;

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
    int pc_loc;
    struct Room *rooms;
    CType *type_map;
    int *hard_map;
    int *dist_map;
    int *dist_map_tunnel;
};

//tunneling trait, currently used to control pathfinding algorithm, will eventually
//become a possible trait for dungeon monsters 
typedef enum isTunneling {
    non_tunneling,
    tunneling
} tunnel_trait;

int edit_cell(struct Floor *floor, int x_loc, int y_loc, CType input_type, int input_hardn);

CType get_type(struct Floor *floor, int x_loc, int y_loc);

int get_hardness(struct Floor *floor, int x_loc, int y_loc);

int place_room(struct Floor *floor, struct Room *room);

int check_intersection(struct Floor *floor, struct Room * room);

int add_rooms(struct Floor *floor);

int add_corridors(struct Floor *floor);

int init_floor(struct Floor *newFloor);

int delete_floor(struct Floor *floor);

int print_floor(struct Floor *floor);

#endif
