#ifndef IO_H

#define IO_H

#include <array>
#include <queue>

#include <ncurses.h>

//#include <queue>
class Dungeon;

class IO_handler
{
public:
	static IO_handler *instance();
	static void reset();

	void handle_input();
	void display_dungeon();
	bool quit_status();

	static void queue_message(std::string message);
	static void print_message_queue(unsigned int x, unsigned int y);
	
private:
	static IO_handler *s_instance;

	IO_handler();
	~IO_handler();

	Dungeon *d;
	bool quit;
	int turn_number;
	static void activate_color(int color);
	static void deactivate_color(int color);

	std::queue<std::string> message_queue;

	void display_inventory();
	void display_equipment();
	void display_equip_prompt();
	void display_unequip_prompt();
	void display_drop_prompt();
	void display_expunge_prompt();
	void display_monster_inspect();
	void display_item_inspect();
};
#endif

