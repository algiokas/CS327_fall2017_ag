#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <ncurses.h>

#include "PQueue.h"
#include "Floor.h"


int main(int argc, char *argv[])
{
	srand(time(0));

	int save = 0;
	int load = 0;
	//int fname_arg = 0; //TODO implement file name arguments

	char *next;
	int monsters = 0;

	int argNum = 1;
	for (argNum = 1; argNum < argc; argNum++) {
		if (argv[argNum] == "--save") {
			save = 1;
		}
		else if (argv[argNum] == "--s") {
			save = 1;
		}
		else if (argv[argNum] == "--load") {
			load = 1;
		}
		else if (argv[argNum] == "--l") {
			load = 1;
		}
		else if (argv[argNum], "--nummon") {
			if (argNum + 1 >= argc) {
				throw "Invalid argument: --nummon requires additional numeric argument";
				break;
			}
			else {
				next = argv[argNum + 1];
				std::stringstream ss;
				ss << next;
				ss >> next;
				if (next <= 0) {
					throw "Invalid argument: --nummon requires additional numeric argument";
					break;
				}
			}
		}
		else {
			throw "Invalid argument";
			return 0;
		}
	}


	if (load == 1) {
		Floor *f = new Floor("dungeon");
	}
	else {
		Floor *f = new Floor();
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