#include "io.h"
#include "Floor.h"
#include "PC.h"
#include "NPC.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <ncurses.h>



IO_handler::IO_handler(Floor *floor)
{
	this->f = floor;
	quit = false;

	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

IO_handler::~IO_handler()
{
	endwin();
}


void IO_handler::handle_input()
{
	bool end = false;
	int key;

	do {
		switch (key = getch()) {
		case '1':
			end = false;
			break;

		case 'q':
			quit = true;
			end = true;
			break;

		default:
			end = true;
			break;
		}
	} while (!end);
}

void IO_handler::display_dungeon()
{
	int y, x;

	clear();
	for (y = 0; y < f->get_height(); y++) {
		for (x = 0; x < f->get_width(); x++) {
			if (f->get_character(x, y)) {
				mvaddch(y + 1, x, f->get_character(x, y)->symbol());
			}
			else {
				switch (f->get_type(x, y)) {
				case rock_c:
				case immutable_c:
					mvaddch(y + 1, x, ' ');
					break;
				case room_c:
					mvaddch(y + 1, x, '.');
					break;
				case corridor_c:
					mvaddch(y + 1, x, '#');
					break;
				case error_c:
					mvaddch(y + 1, x, '*');
					break;
				case stairs_up_c:
					mvaddch(y + 1, x, '<');
					break;
				case stairs_down_c:
					mvaddch(y + 1, x, '>');
					break;
				default:
					/* Use zero as an error symbol, since it stands out somewhat, and it's *
					* not otherwise used.                                                 */
					mvaddch(y + 1, x, '0');
				}
			}
		}
	}
	std::stringstream ss;
	ss << "PC position is ( " << f->get_pc_location().x << ", " << f->get_pc_location().y << ").";
	std::string position = ss.str();
	mvprintw(23, 1, position.c_str());
}

bool IO_handler::quit_status()
{
	return quit;
}
