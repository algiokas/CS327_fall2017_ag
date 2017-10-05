#ifndef DUNGEON_H

#define DUNGEON_H

//Floor dimensions
#define FWIDTH  80 //floor width
#define FHEIGHT 21 //floor height

//Room generation values
#define MINROOMS  5 //minimum rooms per floor as per assignment spec
#define MAXROOMS  15 //arbitrary upper bound 
#define MINROOMWIDTH 3 //minimum width of a room
#define MINROOMHEIGHT 2 //minimum height of a room
#define ROOMDENSITY 0.12 //the fraction of the floor that must be rooms

//Corridor generation values
#define HMODVAL 100 //indicates how much the corridor pathfinding should avoid rooms

//File IO values
#define RPATH "/.rlg327/dungeon" //the relative path from the home directory to the save/load file
#define PVERSION 0 //the program version

//converts the 2D representation of a location into a linear array index
#define INDEX2D(x, y) (FWIDTH * y) + x 

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



//Inline min and max functions to avoid some issues with the corresponding macros
inline int min_int(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}

inline int max_int(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

void set_floor_debug(int i);

int edit_cell(struct Floor *floor, int x_loc, int y_loc, CType input_type, int input_hardn);

CType get_type(struct Floor *floor, int x_loc, int y_loc);

int get_hardness(struct Floor *floor, int x_loc, int y_loc);

int place_room(struct Floor *floor, struct Room *room);

int check_intersection(struct Floor *floor, struct Room * room);

int add_rooms(struct Floor *floor);

int get_neighbors(int index, int *n);

int dijkstra_map(struct Floor *floor, tunnel_trait t);

int dijkstra_corridor(struct Floor *floor, struct Duo source, struct Duo target, int *path);

int draw_path(struct Floor *floor, int len, int *path);

int add_corridors(struct Floor *floor);

int spawn_pc(struct Floor *floor);

int init_floor(struct Floor *newFloor);

int delete_floor(struct Floor *floor);

int print_dist(struct Floor *floor, tunnel_trait t);

int print_floor(struct Floor *floor);

int debug_floor(struct Floor *floor);

int do_load(struct Floor *floor, char *filename);

int do_save(struct Floor *floor);

#endif
