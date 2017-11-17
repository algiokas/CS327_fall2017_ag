#include "IOhandler.h"
#include "Dungeon.h"
#include "PC.h"
#include "NPC.h"
#include "Definitions.h"
#include "Object.h"

#include <cstdlib>
#include <iostream>
#include <sstream>



IO_handler *IO_handler::s_instance = 0;

IO_handler::IO_handler()
{
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


IO_handler * IO_handler::instance()
{
	if (!s_instance) {
		s_instance = new IO_handler();
	}
	return s_instance;
}

void IO_handler::reset()
{
	if (s_instance) {
		delete s_instance;
	}
}

void IO_handler::handle_input()
{
	bool fail = true;
	int key;

	do {
		switch (key = getch()) {
		case '7':
		case 'y':
		case KEY_HOME:
			fail = !Dungeon::instance()->move_pc(northwest);
			break;
		case '8':
		case 'k':
		case KEY_UP:
			fail = !Dungeon::instance()->move_pc(north);
			break;
		case '9':
		case 'u':
		case KEY_PPAGE:
			fail = !Dungeon::instance()->move_pc(northeast);
			break;
		case '6':
		case 'l':
		case KEY_RIGHT:
			fail = !Dungeon::instance()->move_pc(east);
			break;
		case '3':
		case 'n':
		case KEY_NPAGE:
			fail = !Dungeon::instance()->move_pc(southeast);
			break;
		case '2':
		case 'j':
		case KEY_DOWN:
			fail = !Dungeon::instance()->move_pc(south);
			break;
		case '1':
		case 'b':
		case KEY_END:
			fail = !Dungeon::instance()->move_pc(southwest);
			break;
		case '4':
		case 'h':
		case KEY_LEFT:
			fail = !Dungeon::instance()->move_pc(west);
			break;
		case 'd':
			display_drop_prompt();
			break;
		case 'x':
			display_expunge_prompt();
			break;
		case 'i':
			display_inventory();
			fail = true;
			break;
		case 'e':
			display_equipment();
			fail = true;
			break;
		case 'I':
			//inspect an item
			display_item_inspect();
			break;
		case 'L':
			//Inspect monster
			break;
		case 's':
			//return to normal dungeon view
			display_dungeon();
			fail = true;
			break;
		case 't':
			//unequip an item
			display_unequip_prompt();
			break;
		case 'w':
			//equip an item 
			display_equip_prompt();
			break;
		case '>':
			fail = !Dungeon::instance()->change_floor_down();
			break;
		case '<':
			fail = !Dungeon::instance()->change_floor_up();
			break;
		case 'q':
			s_instance->quit = true;
			fail = false;
			break;
		case ']':
			PC::instance()->tgm();
			fail = false;
			break;

		default:
			std::string invalid_key = "Invalid Key : ";
			invalid_key += std::to_string((int)key);
			queue_message(invalid_key);
			display_dungeon();
			fail = true;
			break;
		}
	} while (fail);
}

void IO_handler::queue_message(std::string message)
{
	s_instance->message_queue.push(message);
}

void IO_handler::print_message_queue(unsigned int x, unsigned int y)
{
	if (s_instance->message_queue.size() == 0) {
		return;
	}
	std::string message = s_instance->message_queue.front();
	s_instance->message_queue.pop();
	message = message.substr(0, 71);

	mvprintw(y, x, message.c_str());

	if (s_instance->message_queue.size()) {
		mvprintw(y, x + 70, " --more--");
	}

}

std::string pc_health_bar() {
	int hp = PC::instance()->get_hp();
	double hp_percent = ((double)hp / pc_max_hitpoints) * 100;
	std::string out = "|";
	for (int i = 0; i < 100; i += 5) {
		if (i < hp_percent) {
			out = out + "=";
		}
		else {
			out = out + " ";
		}
	}
	out = out + "|";
	return out;
}

std::string pc_location_string() {
	std::string location = "(x=";
	location += std::to_string(PC::instance()->x_pos());
	location += ", y=";
	location += std::to_string(PC::instance()->y_pos());
	location += ")";
	return location;
}

void IO_handler::display_dungeon()
{
	int y, x;
	bool visible;
	duo pc_loc = Dungeon::instance()->get_pc_location();
	Character *c;
	Object *o;

	clear();
	for (y = 0; y < Dungeon::instance()->get_height(); y++) {
		for (x = 0; x < Dungeon::instance()->get_width(); x++) {
			if ((visible = Dungeon::instance()->pc_can_see(x, y))) {
				attron(A_BOLD);
			}
            if (!Dungeon::instance()->pc_has_seen(x, y)) {
				mvaddch(y + 1, x, ' ');
			}
			else if (pc_loc.x == x && pc_loc.y == y) {
				mvaddch(y + 1, x, '@');
			}
			else if (visible && (c = Dungeon::instance()->get_character(x, y))) {
				activate_color(((NPC *)c)->get_color());
				mvaddch(y + 1, x, c->get_symbol());
				deactivate_color(((NPC *)c)->get_color());
			}
			else if (visible && (o = Dungeon::instance()->get_object(x, y))) {
                activate_color(o->color);
				mvaddch(y + 1, x, object_symbol[o->type]);
                deactivate_color(o->color);
			}
			else {
				switch (Dungeon::instance()->get_type(x, y)) {
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
	mvprintw(23, 1, "HP: ");
	mvprintw(23, 5, std::to_string(PC::instance()->get_hp()).c_str());
	mvprintw(23, 9, pc_health_bar().c_str());

	mvprintw(23, 68, pc_location_string().c_str());

	print_message_queue(0, 0);

	refresh();

	s_instance->turn_number++;
}

bool IO_handler::quit_status()
{
	return s_instance->quit;
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

void IO_handler::display_inventory()
{
	WINDOW *inventory_win = newwin(17, 40, 1, 0);
	box(inventory_win, 0, 0);
	mvwprintw(inventory_win, 1, 1, "Inventory");


	for (int s = 0; s < 10; s++) {
		std::string slot_display = std::to_string(s);
		slot_display += ") ";
		if (PC::instance()->inventory_object(s)) {
			slot_display += PC::instance()->inventory_object(s)->name;
		}
		else {
			slot_display += "Empty slot";
		}
		if (slot_display.length() > 30) {
			slot_display = slot_display.substr(0, 27);
			slot_display += "...";
		}
		mvwprintw(inventory_win, 3 + s, 1, slot_display.c_str());
	}

	refresh();
	wrefresh(inventory_win);

	delwin(inventory_win);
}

std::string equipment_slot_names[] = {
	"Weapon", "Offhand", "Ranged", "Armor", "Helmet", "Cloak",
	"Gloves", "Boots", "Amulet", "Light", "Left ring", "right ring"
};

void IO_handler::display_equipment()
{
	
	WINDOW *equipment_win = newwin(17, 40, 1, 40);
	box(equipment_win, 0, 0);
	mvwprintw(equipment_win, 1, 1, "Equipment");


	for (int s = 0; s < NUM_EQUIPMENT_SLOTS; s++) {
		std::string slot_display = "";
		if (s < 10) {
			slot_display += std::to_string(s);
		}
		else if (s == 10) {
			slot_display += "-";
		}
		else if (s == 11) {
			slot_display += "=";
		}
		slot_display += " ";
		slot_display += equipment_slot_names[s];
		slot_display += ") ";
		if (PC::instance()->equipped_item(static_cast<equipment_slots>(s))) {
			slot_display += PC::instance()->equipped_item(static_cast<equipment_slots>(s))->name;
		}
		else {
			slot_display += "Empty slot";
		}

		mvwprintw(equipment_win, 3 + s, 1, slot_display.c_str());
	}

	refresh();
	wrefresh(equipment_win);

	delwin(equipment_win);
}


void IO_handler::display_equip_prompt()
{
	display_inventory();
	WINDOW *equip_win = newwin(6, 40, 18, 0);
	box(equip_win, 0, 0);
	mvwprintw(equip_win, 1, 1, "Equip item in which slot (0-9)?");
	mvwprintw(equip_win, 2, 20, "press s to cancel");

	refresh();
	wrefresh(equip_win);
	char key;
	do {
		key = getch();
		if (key == 's') {
			display_dungeon();
			return;
		}
		if (!isdigit(key)) {
			mvwprintw(equip_win, 2, 1, "Invalid Slot");
			wrefresh(equip_win);
		}
	} while (!isdigit(key));
	
	int slot = (key - '0');
	PC::instance()->equip_object(slot);

	delwin(equip_win);
}

void IO_handler::display_unequip_prompt()
{
	
	display_equipment();
	WINDOW *unequip_win = newwin(6, 40, 18, 0);
	box(unequip_win, 0, 0);
	mvwprintw(unequip_win, 1, 1, "Unequip item in which slot (0-9)?");
	mvwprintw(unequip_win, 2, 20, "press s to cancel");

	refresh();
	wrefresh(unequip_win);
	char key;
	do {
		key = getch();
		if (key == 's') {
			display_dungeon();
			return;
		}
		if (!isdigit(key) && key != '-' && key != '=') {
			mvwprintw(unequip_win, 2, 1, "Invalid Slot");
			wrefresh(unequip_win);
		}
	} while (!isdigit(key) && key != '-' && key != '=');
	int slot = 0;
	if (isdigit(key)) {
		slot = (key - '0');
	}
	else if (key == '-') {
		slot = 10;
	}
	else if (key == '=') {
		slot = 11;
	}
	PC::instance()->unequip_object((equipment_slots)slot);

	delwin(unequip_win);
}

void IO_handler::display_drop_prompt()
{
	display_inventory();
	WINDOW *drop_win = newwin(6, 40, 18, 0);
	box(drop_win, 0, 0);
	mvwprintw(drop_win, 1, 1, "Drop item in which slot (0-9)?");
	mvwprintw(drop_win, 2, 20, "press s to cancel");

	refresh();
	wrefresh(drop_win);
	char key;
	do {
		key = getch();
		if (key == 's') {
			display_dungeon();
			return;
		}
		if (!isdigit(key)) {
			mvwprintw(drop_win, 2, 1, "Invalid Slot");
			wrefresh(drop_win);
		}
	} while (!isdigit(key));

	int slot = (key - '0');
	PC::instance()->drop_item(slot);

	delwin(drop_win);
}

void IO_handler::display_expunge_prompt()
{
	display_inventory();
	WINDOW *win = newwin(6, 40, 18, 0);
	box(win, 0, 0);
	mvwprintw(win, 1, 1, "Expunge item in which slot (0-9)?");
	mvwprintw(win, 2, 20, "press s to cancel");

	refresh();
	wrefresh(win);
	char key;
	do {
		key = getch();
		if (key == 's') {
			display_dungeon();
			return;
		}
		if (!isdigit(key)) {
			mvwprintw(win, 2, 1, "Invalid Slot");
			wrefresh(win);
		}
	} while (!isdigit(key));

	int slot = (key - '0');
	PC::instance()->expunge_item(slot);

	delwin(win);
}

void IO_handler::display_monster_inspect()
{
}

void inspect_window(Object *obj)
{
	if (!obj) {
		return;
	}
	WINDOW *win = newwin(23, 40, 1, 40);

	mvwprintw(win, 1, 0, obj->name.c_str());
	mvwprintw(win, 2, 0, obj->description.c_str());
	mvwprintw(win, 22, 0, "s to close");

	wrefresh(win);

	delwin(win);
	
}
void IO_handler::display_item_inspect()
{
	display_inventory();
	WINDOW *inspect_win = newwin(6, 40, 18, 0);
	box(inspect_win, 0, 0);
	mvwprintw(inspect_win, 1, 1, "Inspect the item in which slot (0-9)?");
	mvwprintw(inspect_win, 2, 20, "press s to cancel");

	refresh();
	wrefresh(inspect_win);
	char key;
	do {
		key = getch();
		if (key == 's') {
			display_dungeon();
			delwin(inspect_win);
			return;
		}
		if (!isdigit(key)) {
			mvwprintw(inspect_win, 2, 1, "Invalid Slot");
			wrefresh(inspect_win);
		}
	} while (!isdigit(key));

	int slot = (key - '0');
	Object *obj;
	if ((obj = PC::instance()->inventory_object(slot))) {
		inspect_window(obj);
	}
	else {
		queue_message("Empty slot");
		display_dungeon();
		delwin(inspect_win);
		return;
	}
	

	delwin(inspect_win);
}


