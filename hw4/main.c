/*  main.c : Alex Giokas
    Iowa State University - Com Sci 327 - Fall 2017
    contains the main method and */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <ncurses.h>

#include "dungeon.h"
#include "pqueue.h"
#include "path.h"


int main(int argc, char *argv[])
{
    srand(time(NULL));
    struct Floor newFloor;
    
    int debug_output = 0;
    int save = 0;
    int load = 0;
    //int fname_arg = 0; //TODO implement file name arguments
	
	char *next;
	int monsters = 0;

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
        } else if (!strcmp(argv[argNum], "--DEBUG")) {
            debug_output = 1;
		} else if (!strcmp(argv[argNum], "--nummon")) {
			if (argNum + 1 >= argc) {
				printf("Invalid argument: --nummon requires additional numeric argument\n");
				break;
			} else {
				next = argv[argNum + 1];
				monsters = atoi(next);
				if (!monsters) {
					printf("Invalid argument: --nummon requires additional numeric argument\n");
					break;
				}
			}
        } else {
            printf("Invalid argument\n");
            return 0;
        }
    }

    set_floor_debug(debug_output);
    
    if (load) {
        do_load(&newFloor, RPATH); 
        spawn_pc(&newFloor);
        printf("LOAD DUNGEON\n");
    } else { 
        init_floor(&newFloor);
        add_rooms(&newFloor);
        add_corridors(&newFloor);
        spawn_pc(&newFloor);
    } 
    if (debug_output) { debug_floor(&newFloor); }
	
	initscr();
	display_floor(&newFloor);
    
    printf("Press Enter to Exit:\n");
    getchar();
    
    if (save) {
        do_save(&newFloor);
        printf("SAVE DUNGEON\n");
    }

    delete_floor(&newFloor);
    return 0;
}
