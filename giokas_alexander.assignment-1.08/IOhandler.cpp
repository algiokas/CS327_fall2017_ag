#include "IOhandler.h"
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
	this->turn_number = 1;

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
	bool fail;
	int key;

	do {
		switch (key = getch()) {
		case '7':
		case 'y':
		case KEY_HOME:
			fail = !f->move_pc(northwest);
			break;
		case '8':
		case 'k':
		case KEY_UP:
			fail = !f->move_pc(north);
			break;
		case '9':
		case 'u':
		case KEY_PPAGE:
			fail = !f->move_pc(northeast);
			break;
		case '6':
		case 'l':
		case KEY_RIGHT:
			fail = !f->move_pc(east);
			break;
		case '3':
		case 'n':
		case KEY_NPAGE:
			fail = !f->move_pc(southeast);
			break;
		case '2':
		case 'j':
		case KEY_DOWN:
			fail = !f->move_pc(south);
			break;
		case '1':
		case 'b':
		case KEY_END:
			fail = !f->move_pc(southwest);
			break;
		case '4':
		case 'h':
		case KEY_LEFT:
			fail = !f->move_pc(west);
			break;
		case '5':
		case ' ':
		case '.':
		case KEY_B2:
			//end = true;
			break;
		case 'q':
			quit = true;
			fail = false;
			break;

		default:
			fail = true;
			break;
		}
	} while (fail);
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
			else if (!f->pc_can_see(x, y)) {
				mvaddch(y + 1, x, ' ');
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
	ss << "PC position : (" << f->get_pc_location().x << ", " << f->get_pc_location().y << ") Turn : " << turn_number;
	std::string position = ss.str();
	mvprintw(23, 1, position.c_str());
	turn_number++;
}

void IO_handler::print_dungeon()
{
	f->print_floor();
}

bool IO_handler::quit_status()
{
	return quit;
}
