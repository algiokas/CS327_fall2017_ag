#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string.h>
#include <ncurses.h>

#include "PQueue.h"
#include "Floor.h"
#include "Character.h"
#include "PC.h"


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
		if (strcmp(argv[argNum], "--save")) {
			save = 1;
		}
		else if (strcmp(argv[argNum], "--s")) {
			save = 1;
		}
		else if (strcmp(argv[argNum], "--load")) {
			load = 1;
		}
		else if (strcmp(argv[argNum], "--l")) {
			load = 1;
		}
		else if (strcmp(argv[argNum],"--nummon")) {
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

	Floor *f;
	duo spawn_point = f->rand_room_location();
	PC *player = new PC(spawn_point.x, spawn_point.y);
	if (load == 1) {
		f = new Floor(player, "dungeon");
	}
	else {
		f = new Floor(player);
	}

	f->print_floor();

	if (save) {
		f->save_to_file();
	}

	delete f;
	delete player;
	return 0;
}