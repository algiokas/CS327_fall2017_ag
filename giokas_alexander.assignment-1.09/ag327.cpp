#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cstring>
#include <ncurses.h>

#include "PQueue.h"
#include "Dungeon.h"
#include "Character.h"
#include "PC.h"
#include "IOhandler.h"
#include "Definitions.h"


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
				std::cout << "Invalid argument: --nummon requires additional numeric argument";
				break;
			}
			else {
				next = argv[argNum + 1];
				std::stringstream ss;
				ss << next;
				ss >> monsters;
				if (monsters <= 0) {
					std::cout << "Invalid argument: --nummon requires additional numeric argument";
					break;
				}
			}
		}
		else {
			std::cout << "Invalid argument";
			return 0;
		}
	}
	Dungeon::instance();
	PC::instance();
	
	if (load == 1) {
		Dungeon::init("dungeon");
	}
	else {
		Dungeon::init();
	}

	IO_handler::instance()->display_dungeon();

	while (PC::instance()->is_alive() && 
		Dungeon::instance()->boss_ptr &&
		!IO_handler::instance()->quit_status()) {

		Dungeon::instance()->game_turn();
	}
	IO_handler::instance()->display_dungeon();

	if (save) {
		Dungeon::instance()->save_to_file();
	}

	Dungeon::reset();
	IO_handler::reset();
	PC::reset();

	return 0;
}
