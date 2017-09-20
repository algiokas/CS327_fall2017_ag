/*  Dungeon.c : Alex Giokas
    Iowa State University - Com Sci 327 - Fall 2017
    Handles the dungeon state and functions that modify it */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#include "dungeon.h"
#include "pqueue.h"

#define FWIDTH  80 //floor width
#define FHEIGHT 21 //floor height

#define MINROOMS  5 //minimum rooms per floor as per assignment spec
#define MAXROOMS  15 //arbitrary upper bound 
#define MINROOMWIDTH 3 //minimum width of a room
#define MINROOMHEIGHT 2 //minimum height of a room
#define ROOMDENSITY 0.07 //the fraction of the floor that must be rooms

#define HMODVAL 100 //indicates how much the corridor pathfinding should avoid rooms

#define RPATH "/.rlg327/dungeon" //the relative path from the home directory to the save/load file
#define PVERSION 0 //the program version

//converts the 2D representation of a location into a linear array index
#define INDEX2D(x, y) (FWIDTH * y) + x 

int debug_output = 0;

//Inline min and max functions to avoid some issues with the corresponding macros
inline int min(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}

inline int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
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
        if (debug_output) { printf("edit_cell() error - Floor not Initalized"); }
        return -1;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (debug_output) { printf("edit_cell() error - Index out of bounds"); }
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
        if (debug_output) { printf("get_type() error - Floor not Initalized"); }
        return error_c;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (debug_output) { printf("get_type() error - Index out of bounds"); }
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

        if (debug_output) { printf("get_hardness() error - Floor not Initalized"); }
        return -1;
    }
    if (x_loc >= FWIDTH || y_loc >= FHEIGHT) {
        if (debug_output) { printf("get_hardness() error - Floor not Initalized"); }
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
    if (debug_output) { printf("Placing Room...\n"); }
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
    if (debug_output) { printf("Checking Intersection...\n"); }
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
    if (debug_output) { printf("Adding Rooms...\n"); }
    double floorsize = floor->width * floor->height;
    double freespace = floorsize; //the total amount of free space within which to place rooms
    int roomIter = 0;
    struct Room *newRoom; //blank room that we can fill with data and pass to functions
    newRoom = (struct Room *) malloc(sizeof(struct Room));

    while ((floor->numRooms < MINROOMS || (freespace / floorsize) > 1 - ROOMDENSITY)
            && floor->numRooms < MAXROOMS) {
        //Rooms should never touch the edge
        newRoom->loc.x = 1 + (rand() % (FWIDTH - MINROOMWIDTH - 1));
        newRoom->loc.y = 1 + (rand() % (FHEIGHT - MINROOMHEIGHT - 1));
        
        int maxwidth = FWIDTH - newRoom->loc.x - 1; 
        int maxheight = FHEIGHT - newRoom->loc.y - 1;
        if (maxwidth > MINROOMWIDTH) {
            newRoom->dims.x = MINROOMWIDTH + (rand() % (maxwidth - MINROOMWIDTH));
        } else {
            newRoom->dims.x = MINROOMWIDTH;
        }

        if (maxheight > MINROOMWIDTH) {
            newRoom->dims.y = MINROOMHEIGHT + (rand() % (maxheight - MINROOMHEIGHT));
        } else {
            newRoom->dims.y = MINROOMHEIGHT;
        }

        if (!check_intersection(floor, newRoom)) {
            floor->rooms[roomIter] = *newRoom;//newRoom->loc.x;
            floor->numRooms++;
            place_room(floor, newRoom);
            //get the total space taken up by the new room
            int roomsize = newRoom->dims.x + newRoom->dims.y;
            freespace -= roomsize; //and subtract it from free space
            roomIter++;
        }
    }
    free(newRoom);
    return 0;
}


/*  Function: dijkstra_path 
    -----------------------
    Uses Dijkstra's shortest path algorithm to calculate a path between two
    locations in the dungeon. The floor map is treated as a graph where adjacent
    cells are connected, and the edge weights between two cells is equivalent to the
    hardness of the destination cell. Additionally, the HMODVALUE macro in this file
    modifies the behaviour of this algorithm to treat rooms cells as higher than their
    actual hardness. This implementation uses the priority queue in pqueue.c. 
    
    floor: pointer to the floor being modified
    source, target: ordered pairs that denote the start and end of the path
    path: a pointer array that is passed to the function and will contain the output path

    returns: 0 if successful
 */
int dijkstra_path(struct Floor *floor, struct Duo source, struct Duo target, int *path)
{
    if (debug_output) { printf("Calculating Dijkstra Path...\n"); }
    int *dist;
    int *prev;
    int *visited;
    struct PQueue *pq;
    int current;
    int *in = (int *) malloc(sizeof(int)); //vertex pointer for passing vertex data to queue
    int *out = malloc(sizeof(int)); //void pointer for retrieving data from queue

    dist = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    prev = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    visited = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    pq = (struct PQueue *) malloc(sizeof(struct PQueue));
    init_PQ(pq, (FWIDTH * FHEIGHT / 2), sizeof(int));

    int iter;
    int sourceIndex = INDEX2D(source.x, source.y);
    int targetIndex = INDEX2D(target.x, target.y);
    if (debug_output) { printf("DPATH: source=%d, target=%d\n" ,sourceIndex, targetIndex); }

    for (iter = 0; iter < FWIDTH * FHEIGHT; iter++) {
        if (iter == sourceIndex) {
            dist[iter] = 0;
            visited[iter] = 1;
        } else {
            dist[iter] = INT_MAX;
            visited[iter] = 0;
        }
        prev[iter] = -1;
    }

    *in = sourceIndex;
    insert(pq, 0, in);


    while(pq->size) { //queue is not empty
        remove_min(pq, out);
        current = *out;
        if (current == INDEX2D(target.x, target.y)) {
            break;
        }
        //Left neighbor
        if ((current % FWIDTH) > 1) {
            int left = current - 1;
            int hmod = HMODVAL * (floor->type_map[left] == room_c);
            if (dist[left] > dist[current] + floor->hard_map[left] + hmod) {
                dist[left] = dist[current] + floor->hard_map[left] + hmod;
                prev[left] = current;
                if (!visited[left]) {
                    *in = left;
                    insert(pq, dist[left], in);
                    visited[left] = 1;
                }
            }
        }
        //top neighbor
        if ((current / FWIDTH) > 1) {
            int top = current - FWIDTH;
            int hmod = HMODVAL * (floor->type_map[top] == room_c);
            if (dist[top] > dist[current] + floor->hard_map[top] + hmod) {
                dist[top] = dist[current] + floor->hard_map[top] + hmod;
                prev[top] = current;
                if (!visited[top]) {
                    *in = top;
                    insert(pq, dist[top], in);
                    visited[top] = 1;
                }
            }
        }
        //right neighbor
        if ((current % FWIDTH) < FWIDTH - 1) {
            int right = current + 1;
            int hmod = HMODVAL * (floor->type_map[right] == room_c);
            if (dist[right] > dist[current] + floor->hard_map[right] + hmod) {
                dist[right] = dist[current] + floor->hard_map[right] + hmod;
                prev[right] = current;
                if (!visited[right]) {
                    *in = right;
                    insert(pq, dist[right], in);
                    visited[right] = 1;
                }
            }
        }
        //bottom neighbor
        if ((current / FWIDTH) < FHEIGHT - 1) {
            int bottom = current + FWIDTH;
            int hmod = HMODVAL * (floor->type_map[bottom] == room_c);
            if (dist[bottom] > dist[current] + floor->hard_map[bottom] + hmod) {
                dist[bottom] = dist[current] + floor->hard_map[bottom] + hmod;
                prev[bottom] = current;
                if (!visited[bottom]) {
                    *in = bottom;
                    insert(pq, dist[bottom], in);
                    visited[bottom] = 1;
                }
            }
        }
    }

    if (debug_output) { printf("Constructing path from %d to %d\n", current, sourceIndex); }
    int pathIter = 1;
    //printf("test1\n");
    path[0] = current;
    //printf("test2\n");
    while (prev[current] != sourceIndex && pathIter < 100) {
        path[pathIter] = prev[current];
        current = prev[current];
        pathIter++;
    }
    
    delete_PQ(pq);
    free(dist);
    free(prev);
    free(visited);
    free(in);
    free(out);
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
int draw_path(struct Floor *floor, int *path)
{
    if (debug_output) { printf("Drawing Path...\n"); }
    if (!path) {
        if (debug_output) { printf("draw_path() error: Invalid path argument\n"); }
        return 1;
    }
    int iter = 0;
    while(path[iter]) {
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
    if (debug_output) {
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
        if (debug_output) { printf("Room %d to %d path\n", roomIter, nextRoom); }

        roomACentroid.x = floor->rooms[roomIter].loc.x + (floor->rooms[roomIter].dims.x / 2);
        roomACentroid.y = floor->rooms[roomIter].loc.y + (floor->rooms[roomIter].dims.y / 2);
        roomBCentroid.x = floor->rooms[nextRoom].loc.x + (floor->rooms[nextRoom].dims.x / 2);
        roomBCentroid.y = floor->rooms[nextRoom].loc.y + (floor->rooms[nextRoom].dims.y / 2);
        
        dijkstra_path(floor, roomACentroid, roomBCentroid, path);
        draw_path(floor, path);
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
    if (debug_output) { printf("Initializing Floor...\n"); }
    newFloor->width = FWIDTH;
    newFloor->height = FHEIGHT;
    newFloor->numRooms = 0;

    newFloor->rooms = (struct Room *) malloc(MAXROOMS * sizeof(struct Room));
    newFloor->type_map = (CType*) malloc(FWIDTH * FHEIGHT * sizeof(CType));
    newFloor->hard_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));

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
    if (debug_output) { printf("Deleting Floor...\n"); }
    free(floor->rooms);
    free(floor->type_map);
    free(floor->hard_map);
    free(floor);

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
        printf("|\n");
    }
    //bottom edge of border
    for (dash = 0; dash < FWIDTH + 2; dash++) {
        printf("-");
    }
    printf("\n");
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
        printf("test1\n");
    }
    printf("Debug Output End...\n");
}
 
int do_load(struct Floor *floor) {
    if (debug_output) { printf("Loading from file...\n"); }

    char *home; //will be the path of the home directory
    char *rpath; //the path from the home dir to the file
    char *path; //the entire path
    FILE *f;
    char *marker = "RLG327"; //the file type marker for this file
    int32_t version = PVERSION;
    int32_t fsize;

    floor->width = FWIDTH;
    floor->height = FHEIGHT;
    floor->numRooms = 0;

    floor->rooms = (struct Room *) malloc(MAXROOMS * sizeof(struct Room));
    floor->type_map = (CType*) malloc(FWIDTH * FHEIGHT * sizeof(CType));
    floor->hard_map = (int *) malloc(FWIDTH * FHEIGHT * sizeof(int));
    
    home = getenv("HOME");
    rpath = RPATH;
    path = (char *) malloc((strlen(home) + strlen(rpath) + 2) * sizeof(char));
    strcpy(path, home);
    strcat(path, rpath);
    
    //buffers to store the header information for the fil
    unsigned char *marker_buff = (unsigned char *) malloc(7 * sizeof(unsigned char));
    unsigned char *version_buff = (unsigned char *) malloc(5 * sizeof(unsigned char));
    unsigned char *size_buff = (unsigned char *) malloc(5 * sizeof(unsigned char));

    struct Room *temp_room = (struct Room *) malloc(sizeof(struct Room));

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
            if (strcmp(marker_buff, marker) != 0) {
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
            fsize = s;
            if (s < 1694) {
                printf("do_load() warning: invalid file size\n");
            }
        }
        
        //read dungeon cells
        if (i > 13 && i < 1694) {
            int x = ((i - 14) % FWIDTH);
            int y = ((i - 14) / FHEIGHT);
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
                    temp_room->loc.y = ch;
                    break;
                case 1:
                    temp_room->loc.x = ch;
                    break;
                case 2:
                    temp_room->dims.y = ch;
                    break;
                case 3:
                    temp_room->dims.x = ch;
                    int roomNum = (i - 1694) / 4;
                    floor->rooms[roomNum] = *temp_room;
                    floor->numRooms++;
                    place_room(floor, temp_room);
                    break;
            }
        }
        i++;
    }

    free(marker_buff);
    free(version_buff);
    free(size_buff);
    free(temp_room);
    
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
    if (debug_output) { printf("Saving to file...\n"); }
    
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

int main(int argc, char *argv[])
{
    if (argc > 2) {
        printf("Usage: ./dungeon [--save] [--load]\n");
        return 0;
    }
    srand(time(NULL));
    struct Floor *newFloor = (struct Floor *) malloc(sizeof(struct Floor));
    
    int save = 0;
    int load = 0;
    int fname_arg = 0;

    int argNum = 1;
    for (argNum = 1; argNum < argc; argNum++) {
        if (debug_output) { printf("Arg %d: %s \n", argNum, argv[argNum]); }
        if (!strcmp(argv[argNum], "--save")) {
            save = 1;
        } else if (!strcmp(argv[argNum], "--s")) {
            save = 1;
        } else if (!strcmp(argv[argNum], "--load")) {
            load = 1;
        } else if (!strcmp(argv[argNum], "--l")) {
            load = 1;
        }
    }

    if (load) {
        do_load(newFloor); 
        printf("LOAD DUNGEON\n");
    } else { 
        init_floor(newFloor);
        add_rooms(newFloor);
        add_corridors(newFloor);
    } 
    if (debug_output) { debug_floor(newFloor); }
    print_floor(newFloor);
    printf("Press Enter to Exit:\n");
    getchar();
    
    if (save) {
        do_save(newFloor);
        printf("SAVE DUNGEON\n");
    }

    delete_floor(newFloor);
    return 0;
}
