#include "IOhandler.h"
#include "Dungeon.h"
#include "PC.h"
#include "NPC.h"
#include "Definitions.h"
#include "Object.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <ncurses.h>



IO_handler::IO_handler(Dungeon *d)
{
	this->d = d;
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
			fail = !d->move_pc(northwest);
			break;
		case '8':
		case 'k':
		case KEY_UP:
			fail = !d->move_pc(north);
			break;
		case '9':
		case 'u':
		case KEY_PPAGE:
			fail = !d->move_pc(northeast);
			break;
		case '6':
		case 'l':
		case KEY_RIGHT:
			fail = !d->move_pc(east);
			break;
		case '3':
		case 'n':
		case KEY_NPAGE:
			fail = !d->move_pc(southeast);
			break;
		case '2':
		case 'j':
		case KEY_DOWN:
			fail = !d->move_pc(south);
			break;
		case '1':
		case 'b':
		case KEY_END:
			fail = !d->move_pc(southwest);
			break;
		case '4':
		case 'h':
		case KEY_LEFT:
			fail = !d->move_pc(west);
			break;
		case '5':
		case ' ':
		case '.':
		case KEY_B2:
			//end = true;
			break;
		case '>':
			fail = !d->change_floor_down();
			break;
		case '<':
			fail = !d->change_floor_up();
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
	bool visible;
	duo pc_loc = d->get_pc_location();
	Character *c;
	Object *o;

	clear();
	for (y = 0; y < d->get_height(); y++) {
		for (x = 0; x < d->get_width(); x++) {
			if ((visible = d->pc_can_see(x, y))) {
				attron(A_BOLD);
			}
            if (!d->pc_has_seen(x, y)) {
				mvaddch(y + 1, x, ' ');
			}
			else if (pc_loc.x == x && pc_loc.y == y) {
				mvaddch(y + 1, x, '@');
			}
			else if ((c = d->get_character(x, y))) {
				activate_color(((NPC *)c)->get_color());
				mvaddch(y + 1, x, c->symbol());
				deactivate_color(((NPC *)c)->get_color());
			}
			else if ((o = d->get_object(x, y))) {
                activate_color(o->color);
				mvaddch(y + 1, x, object_symbol[o->type]);
                deactivate_color(o->color);
			}
			else {
				switch (d->get_type(x, y)) {
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
			if (visible) {
				attroff(A_BOLD);
			}
		}
	}
	std::stringstream ss;
	ss << "PC position : (" << d->get_pc_location().x 
       << ", " << d->get_pc_location().y << ") Turn : " << turn_number;
	std::string position = ss.str();
	mvprintw(23, 1, position.c_str());
	turn_number++;
}

void IO_handler::print_dungeon()
{
	d->print_floor();
}

bool IO_handler::quit_status()
{
	return quit;
}

void IO_handler::activate_color(int color)
{
	switch (color) {
	case 0:
		attron(COLOR_PAIR(COLOR_RED));
		break;
	case 1:
		attron(COLOR_PAIR(COLOR_GREEN));
		break;
	case 2:
		attron(COLOR_PAIR(COLOR_BLUE));
		break;
	case 3:
		attron(COLOR_PAIR(COLOR_CYAN));
		break;
	case 4:
		attron(COLOR_PAIR(COLOR_YELLOW));
		break;
	case 5:
		attron(COLOR_PAIR(COLOR_MAGENTA));
		break;
	case 6:
		attron(COLOR_PAIR(COLOR_WHITE));
		break;
	case 7:
		attron(COLOR_PAIR(COLOR_WHITE));
		break;
	default:
		std::cout << "activate_color() : invalid color index";
	}
}

void IO_handler::deactivate_color(int color)
{
	switch (color) {
	case 0:
		attroff(COLOR_PAIR(COLOR_RED));
		break;
	case 1:
		attroff(COLOR_PAIR(COLOR_GREEN));
		break;
	case 2:
		attroff(COLOR_PAIR(COLOR_BLUE));
		break;
	case 3:
		attroff(COLOR_PAIR(COLOR_CYAN));
		break;
	case 4:
		attroff(COLOR_PAIR(COLOR_YELLOW));
		break;
	case 5:
		attroff(COLOR_PAIR(COLOR_MAGENTA));
		break;
	case 6:
		attroff(COLOR_PAIR(COLOR_WHITE));
		break;
	case 7:
		attroff(COLOR_PAIR(COLOR_WHITE));
		break;
	default:
		std::cout << "activate_color() : invalid color index";
	}
}
