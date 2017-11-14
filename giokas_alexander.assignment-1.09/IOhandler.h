#ifndef IO_H

#define IO_H

#include <array>

//#include <queue>
class Dungeon;

class IO_handler
{
public:
	IO_handler(Dungeon *d);
	~IO_handler();

	void handle_input();
	void display_dungeon();
	void print_dungeon();
	bool quit_status();
	
private:
	Dungeon *d;
	bool quit;
	int turn_number;
	void activate_color(int color);
	void deactivate_color(int color);
	//std::queue<std::string> message_queue;

	//void display_message_queue(int x, int y);
	//void display_monter_list();
	//void scroll_monster_list();
	//void display_primary();
};
#endif

