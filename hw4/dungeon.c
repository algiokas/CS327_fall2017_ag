/*  Dungeon.c : Alex Giokas
    Iowa State University - Com Sci 327 - Fall 2017
    Handles the dungeon state and functions that modify it */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <ncurses.h>

#include "dungeon.h"
#include "pqueue.h"
#include "path.h"


int floor_debug = 0;

//
void set_floor_debug(int i)
{
    floor_debug = i;
}
    
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
int edit_cell(struct Floor *floor, int x_loc, int y_loc, CType input_type, int input_hardn)
{
    
    if (!floor) {
        if (floor_debug) { printf("edit_cell() error - Floor not Initalized"); }
        return -1;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (floor_debug) { printf("edit_cell() error - Index out of bounds"); }
        return -1;
    }
    int cellIndex = (FWIDTH * y_loc) + x_loc;
    floor->type_map[cellIndex] = input_type;
    floor->hard_map[cellIndex] = input_hardn;
    return 0;
}

/*  Function get_type
    -----------------
    takes a floor pointer and the x-y co-ordinates of a cell and returns
    the type of that cell

    floor: The floor from which we are getting a cell
    x_loc, y_loc: the x and y co-ordinates of the cell we are getting

    returns: the CType value of the cell at (x_loc, y_loc)
*/
CType get_type(struct Floor *floor, int x_loc, int y_loc)
{
    if (!floor) {
        if (floor_debug) { printf("get_type() error - Floor not Initalized"); }
        return error_c;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (floor_debug) { printf("get_type() error - Index out of bounds"); }
        return error_c;
    }
    int cellIndex = (FWIDTH * y_loc) + x_loc;

    return floor->type_map[cellIndex];
}



/*  Function get_hardness
    -----------------
    takes a floor pointer and the x-y co-ordinates of a cell and returns
    the hardness of that cell

    floor: The floor from which we are getting a cell
    x_loc, y_loc: the x and y co-ordinates of the cell we are getting

    returns: the hardness value of the cell at (x_loc, y_loc)
*/
int get_hardness(struct Floor *floor, int x_loc, int y_loc)
{
    if (!floor) {

        if (floor_debug) { printf("get_hardness() error - Floor not Initalized"); }
        return -1;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (floor_debug) { printf("get_hardness() error - Floor not Initalized"); }
        return -1;
    }
    int cellIndex = (FWIDTH * y_loc) + x_loc;

    return floor->hard_map[cellIndex];
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
    if (floor_debug) { printf("Placing Room...\n"); }
    int row, col;
    for (col = room->loc.y; col < room->loc.y + room->dims.y; col++) {
        for (row = room->loc.x; row < room->loc.x + room->dims.x; row++) {
            edit_cell(floor, row, col, room_c, 0);
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
    if (floor_debug) { printf("Checking Intersection...\n"); }
    if (!floor->numRooms) {
        return 0;
    }
    int roomIter;
    for (roomIter = 0; roomIter < floor->numRooms; roomIter++) {
        //temporary variables assigned for clarity
        int new_left = room->loc.x;
        int new_right = room->loc.x + room->dims.x;
        int new_top = room->loc.y;
        int new_bottom = room->loc.y + room->dims.y;

        int old_left = floor->rooms[roomIter].loc.x;
        int old_right = floor->rooms[roomIter].loc.x + floor->rooms[roomIter].dims.x;
        int old_top = floor->rooms[roomIter].loc.y;
        int old_bottom = floor->rooms[roomIter].loc.y + floor->rooms[roomIter].dims.y;

        int x_intersect = 0;
        int y_intersect = 0;

        if (old_left <= new_left && new_left <= old_right) { x_intersect = 1; }
        if (old_left <= new_right && new_right <= old_right) { x_intersect = 1; }
        if (new_left <= old_left && old_right <= new_right) { x_intersect = 1; }
        if (old_top <= new_top && new_top <= old_bottom) { y_intersect = 1; }
        if (old_top <= new_bottom && new_bottom <= old_bottom) { y_intersect = 1; }
        if (new_top <= old_top && old_bottom <= new_bottom) { y_intersect = 1; }
        
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
int add_rooms(struct Floor *floor)
{
    if (floor_debug) { printf("Adding Rooms...\n"); }
    double floorsize = floor->width * floor->height;
    double freespace = floorsize; //the total amount of free space within which to place rooms
    int roomIter = 0;
    struct Room newRoom; //blank room that we can fill with data and pass to functions

    while ((floor->numRooms < MINROOMS || (freespace / floorsize) > 1 - ROOMDENSITY)
            && floor->numRooms < MAXROOMS) {
        //Rooms should never touch the edge
        newRoom.loc.x = 1 + (rand() % (FWIDTH - MINROOMWIDTH - 1));
        newRoom.loc.y = 1 + (rand() % (FHEIGHT - MINROOMHEIGHT - 1));
        
        int maxwidth = FWIDTH - newRoom.loc.x - 1; 
        int maxheight = FHEIGHT - newRoom.loc.y - 1;
        if (maxwidth > MINROOMWIDTH) {
            newRoom.dims.x = MINROOMWIDTH + (rand() % (maxwidth - MINROOMWIDTH));
        } else {
            newRoom.dims.x = MINROOMWIDTH;
        }

        if (maxheight > MINROOMWIDTH) {
            newRoom.dims.y = MINROOMHEIGHT + (rand() % (maxheight - MINROOMHEIGHT));
        } else {
            newRoom.dims.y = MINROOMHEIGHT;
        }

        if (!check_intersection(floor, &newRoom)) {
            floor->rooms[roomIter] = newRoom;
            floor->numRooms++;
            place_room(floor, &newRoom);
            //get the total space taken up by the new room
            int roomsize = newRoom.dims.x * newRoom.dims.y;
            freespace -= roomsize; //and subtract it from free space
            roomIter++;
        }
        if (floor_debug) { printf("Current room density: %f\n", (freespace / floorsize)); }
    }
    return 0;
}

int get_neighbors(int index, int *n)
{
    int left = index - 1;
    int top = index - FWIDTH;
    int right = index + 1;
    int bottom = index + FWIDTH;
    int topleft = top - 1;
    int topright = top + 1;
    int botright = bottom + 1;
    int botleft = bottom - 1;
        
    *n = left;
    *(n+1) = topleft;
    *(n+2) = top;
    *(n+3) = topright;
    *(n+4) = right;
    *(n+5) = botright;
    *(n+6) = bottom; 
    *(n+7) = botleft;

    return 0;
}
    



/*  Function: draw_path 
    -------------------
    Takes in a path as an int pointer and iterates along the path, modifying
    each rock cell along the path to be a corridor cell.
    
    floor: pointer to the floor being modified
    path: int array containing the path

    returns: 0 if successful
*/
int draw_path(struct Floor *floor, int len, int *path)
{
    if (floor_debug) { printf("Drawing Path...\n"); }
    if (!path) {
        if (floor_debug) { printf("draw_path() error: Invalid path argument\n"); }
        return 1;
    }
    int iter = 0;
    //while(path[iter]) {
    while(iter < len) {
        int pathx = (path[iter] % FWIDTH);
        int pathy = (path[iter] / FWIDTH);
        if (get_type(floor, pathx, pathy) == rock_c) {
            edit_cell(floor, pathx, pathy, corridor_c, 0);
        }
        iter++;
    }
    return 0;
}



/*  Function: add_corridors
    -----------------------
    Uses dijkstra's algorithm to add corridors between each consecutive room
    in a floor's dynamic rooms array

    floor: pointer to the floor being modified
*/
int add_corridors(struct Floor *floor)
{
    if (floor_debug) {
        printf("Adding Corridors...\n");
        printf("NumRooms: %d\n", floor->numRooms);
    }

    int roomIter, nextRoom;
    int *path = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));

    for (roomIter = 0; roomIter < floor->numRooms; roomIter++) {
        struct Duo roomACentroid, roomBCentroid;
        if (roomIter == floor->numRooms-1) {
            nextRoom = 0;
        } else {
            nextRoom = roomIter + 1;
        }
        if (floor_debug) { printf("Room %d to %d path\n", roomIter, nextRoom); }

        roomACentroid.x = floor->rooms[roomIter].loc.x + (floor->rooms[roomIter].dims.x / 2);
        roomACentroid.y = floor->rooms[roomIter].loc.y + (floor->rooms[roomIter].dims.y / 2);
        roomBCentroid.x = floor->rooms[nextRoom].loc.x + (floor->rooms[nextRoom].dims.x / 2);
        roomBCentroid.y = floor->rooms[nextRoom].loc.y + (floor->rooms[nextRoom].dims.y / 2);
        
        int path_len = dijkstra_corridor(floor, roomACentroid, roomBCentroid, path);
        draw_path(floor, path_len, path);
    }
    free(path);
    return 0;
}

    

/*  Function: init_floor 
    --------------------
    Initalizes a floor pointer by setting starting values and allocating memory for
    pointer members. Also sets the values in map to default values, namely immutable
    around the edges and rock with random hardness throughout

    newFloor: pointer to the floor being initialized

    returns: 0 if successful
*/
int init_floor(struct Floor *newFloor)
{
    if (floor_debug) { printf("Initializing Floor...\n"); }
    newFloor->width = FWIDTH;
    newFloor->height = FHEIGHT;
    newFloor->numRooms = 0;
    newFloor->pc_loc = 0;

    newFloor->rooms = (struct Room *) malloc(MAXROOMS * sizeof(struct Room));
    newFloor->type_map = (CType*) malloc(FWIDTH * FHEIGHT * sizeof(CType));
    newFloor->hard_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    newFloor->dist_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    newFloor->dist_map_tunnel = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));

    int celliter;
    for (celliter = 0; celliter < (FWIDTH * FHEIGHT); celliter++) {
        if (celliter < FWIDTH || (celliter % FWIDTH) == 0 ||
            (celliter % FWIDTH) == FWIDTH - 1 || celliter > (FWIDTH) * (FHEIGHT-1)) {
            newFloor->type_map[celliter] = immutable_c;
            newFloor->hard_map[celliter] = 255;
        } else {
            int rand_hardness = 1 + (rand() % 254);
            newFloor->type_map[celliter] = rock_c;
            newFloor->hard_map[celliter] = rand_hardness;
        }
    }
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
    if (floor_debug) { printf("Deleting Floor...\n"); }
    free(floor->rooms);
    free(floor->type_map);
    free(floor->hard_map);

    return 0;
}

//prints distance maps either tunneling or non-tunneling based on the parameter t
//used only for homework 3
int print_dist(struct Floor *floor, tunnel_trait t)
{
    int row, col, dash, idx;
    printf("\nDistance Map\n");
    //top edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
    for (col = 0; col < FHEIGHT; col++) {
        printf("|");
        for (row = 0; row < FWIDTH; row++) {
            idx = INDEX2D(row, col); 
            if (idx == floor->pc_loc) {
                printf("@");
            }
            else if (t == non_tunneling) {
                if (floor->dist_map[idx] == INT_MAX) {
                    printf(" ");
                } else if (floor->dist_map[idx]) {
                    printf("%d", floor->dist_map[idx] % 10);
                } else {
                    printf("X"); //error character
                }
            }
            else if (t == tunneling) {
                if (row == 0 || row == FWIDTH-1 || col == 0 || col == FHEIGHT-1) {
                    printf(" ");
                } else if (floor->dist_map_tunnel[idx]) {
                    printf("%d", floor->dist_map_tunnel[idx] % 10);
                } else {
                    printf("X"); //error character
                }
            }
        }
        printf("|\n");
    }
    //bottom edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
    return 0;
}

/*  Function: print_floor
    ---------------------
    prints the map of a floor to the console with a border

    floor: a pointer to the floor to be printed
    
    returns: 0 if successful
*/
int print_floor(struct Floor *floor)
{
    int row, col, dash;
    printf("\nDungeon Map\n");
    //top edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
    for (col = 0; col < FHEIGHT; col++) {
        printf("|");
        for (row = 0; row < FWIDTH; row++) {
            if (INDEX2D(row, col) == floor->pc_loc) {
                printf("@");
            } else { 
                switch(get_type(floor, row, col)) {
                    case(immutable_c) :
                        printf(" ");
                        break;
                    case(rock_c) :
                        printf(" ");
                        break;
                    case(room_c) :
                        printf(".");
                        break;
                    case(corridor_c) :
                        printf("#");
                        break;
                    default :
                        printf("X"); //indicates an error
                }
            }
        }
        printf("|\n");
    }
    //bottom edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
    return 0;
}

int display_floor(struct Floor *floor)
{
	int row, col, dash;
	move(1, 0);
    for (row = 0; row < FHEIGHT; row++) {
        for (col = 0; col < FWIDTH; col++) {
            if (INDEX2D(row, col) == floor->pc_loc) {
                addch("@");
            } else { 
                switch(get_type(floor, row, col)) {
                    case(immutable_c) :
                        addch(' ');
                        break;
                    case(rock_c) :
                        addch(' ');
                        break;
                    case(room_c) :
                        addch('.');
                        break;
                    case(corridor_c) :
                        addch('#');
                        break;
                    default :
                        addch("X"); //indicates an error
                }
            }
        }
		move(row, 0);
    }
    return 0;
}

//local function for debugging the output
int debug_floor(struct Floor *floor) {
    printf("Width: %d\n", floor->width);
    printf("Height: %d\n", floor->height);
    printf("Number of Rooms: %d \n", floor->numRooms);
    int i;
    for (i = 0; i < floor->numRooms; i++) {
        printf("Floor number %d \n", i);
        printf("    location: (%d, %d) \n", floor->rooms[i].loc.x, floor->rooms[i].loc.y);
        printf("    Dimensions: %dx%d \n", floor->rooms[i].dims.x, floor->rooms[i].dims.y);
    }
    printf("Debug Output End...\n");
    return 0;
}
 
int do_load(struct Floor *floor, char *filename) {
    if (floor_debug) { printf("Loading from file...\n"); }

    char *home; //will be the path of the home directory
    char *rpath; //the path from the home dir to the file
    char *path; //the entire path
    FILE *f;
    const char *marker = "RLG327"; //the file type marker for this file
    int32_t version = PVERSION;
    //int32_t fsize;
    int roomCapacity = MAXROOMS;

    floor->width = FWIDTH;
    floor->height = FHEIGHT;
    floor->numRooms = 0;
    floor->pc_loc = 0;

    floor->rooms = (struct Room *) malloc(roomCapacity * sizeof(struct Room));
    floor->type_map = (CType*) malloc(FWIDTH * FHEIGHT * sizeof(CType));
    floor->hard_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    floor->dist_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    floor->dist_map_tunnel = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    
    home = getenv("HOME");
    rpath = filename;
    path = (char *) malloc((strlen(home) + strlen(rpath) + 2) * sizeof(char));
    strcpy(path, home);
    strcat(path, rpath);
    
    //buffers to store the header information for the fil
    unsigned char marker_buff[7];
    unsigned char version_buff[5];
    unsigned char size_buff[5];

    struct Room temp_room;

    f = fopen(path, "r");

    free(path);
    
    int i = 0;
    int ch; //we need this to be an int so we can check for EOF
    while ((ch = fgetc(f)) != EOF) {
        //handle file marker
        if (i < 6) {
            marker_buff[i] = (unsigned char) ch;
        } else if (i == 6) {
            marker_buff[6] = '\0';
            if (strcmp((const char *) marker_buff, marker) != 0) {
                printf("do_load() warning: Invalid file marker\n");
            }
        }
        
        //handle version number
        if (i > 5 && i < 10) {
            version_buff[9 - i] = (unsigned char) ch; //place bytes in reverse order 
        } else if (i == 10) {
            version_buff[4] = '\0';
            int32_t v;
            memcpy(&v, version_buff, sizeof(int32_t));
            if (v != version) {
                printf("do_load() warning: Different file version\n");
            }
        }

        //handle file size
        if (i > 9 && i < 14) {
            size_buff[13 - i] = (unsigned char) ch; //place bytes in reverse order 
        } else if (i == 14) {
            size_buff[4] = '\0';
            int32_t s;
            memcpy(&s, size_buff, sizeof(int32_t));
            //fsize = s;
            if (s < 1694) {
                printf("do_load() warning: invalid file size\n");
            }
        }
        
        //read dungeon cells
        if (i > 13 && i < 1694) {
            floor->hard_map[i - 14] = ch;
            if (ch == 255) {
                floor->type_map[i - 14] = immutable_c;
            } else if (ch == 0) {
                floor->type_map[i - 14] = corridor_c;
            } else {
                floor->type_map[i - 14] = rock_c;
            }
        }

        if (i > 1693) {
            switch((i - 1694) % 4) {
                case 0:
                    temp_room.loc.y = ch;
                    break;
                case 1:
                    temp_room.loc.x = ch;
                    break;
                case 2:
                    temp_room.dims.y = ch;
                    break;
                case 3:
                    temp_room.dims.x = ch;
                    int roomNum = (i - 1694) / 4;
                    floor->rooms[roomNum] = temp_room;
                    floor->numRooms++;

                    if (floor->numRooms + 1 >= roomCapacity) {
                        struct Room *temp_rooms;
                        temp_rooms = (struct Room *) realloc(floor->rooms, 
                                     2 * roomCapacity * sizeof(struct Room));
                        if (temp_rooms == NULL) {
                            printf("Error in do_load(): realloc failure\n");
                        } else {
                            floor->rooms = temp_rooms;
                            roomCapacity = roomCapacity * 2;
                        }
                    }
                    place_room(floor, &temp_room);
                    break;
            }
        }
        i++;
    }
    spawn_pc(floor);
    fclose(f);
    return 0;
}

//Local wrapper function for fwrite() that writes 32 bit data in big endian form
//note, this function is based off of a stackoverflow answer but the structure is
//simple enough that it seemed ok to include
size_t write_bigendian(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (size != 4) {
        printf("write_bigendian() error - invalid data size"); 
        return 0;
    }
    unsigned char *output = (unsigned char *) ptr;

    int e;
    for (e = 0; e < nmemb; e++) {
        unsigned char byte0 = output[e * size];
        unsigned char byte1 = output[(e * size) + 1];
        output[e * size] = output[(e * size) + 3];
        output[(e * size) + 1] = output[(e * size) + 2];
        output[(e * size) + 2] = byte1;
        output[(e * size) + 3] = byte0;
    }

    return fwrite(ptr,  size, nmemb, stream);
}

    
    
int do_save(struct Floor *floor) {
    if (floor_debug) { printf("Saving to file...\n"); }
    
    char *home; //will be the path of the home directory
    char *rpath; //the path from the home dir to the file
    char *path; //the entire path
    FILE *f;
    char *marker = "RLG327"; //the file type marker for this file
    int32_t version = 0;
    int32_t fsize;

    int headersize = (strlen(marker) * sizeof(char)) + sizeof(version) + sizeof(fsize);
    fsize = headersize + (FWIDTH * FHEIGHT) + (4 * floor->numRooms);

    home = getenv("HOME");
    rpath = RPATH;
    path = (char *) malloc((strlen(home) + strlen(rpath) + 2) * sizeof(char));
    strcpy(path, home);
    strcat(path, rpath);

    f = fopen(path, "w");

    free(path);
    
    //begin file write
    fputs(marker, f);
    write_bigendian(&version, sizeof(version), 1, f);
    write_bigendian(&fsize, sizeof(fsize), 1, f);

    int i;
    for (i = 0; i < FWIDTH * FHEIGHT; i++) {
        int x = (i % FWIDTH);
        int y = (i / FWIDTH);
        fputc(get_hardness(floor, x, y), f);
    }

    int room;
    for (room = 0; room < floor->numRooms; room++) {
        fputc(floor->rooms[room].loc.y, f);
        fputc(floor->rooms[room].loc.x, f);
        fputc(floor->rooms[room].dims.y, f);
        fputc(floor->rooms[room].dims.x, f);
    }
    
    fclose(f);
    return 0;
}
