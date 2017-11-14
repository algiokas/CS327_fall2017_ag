#ifndef IO_H

#define IO_H

#include <array>

//#include <queue>
class Floor;


class IO_handler
{
public:
	IO_handler(Floor *floor);
	~IO_handler();

	void handle_input();
	void display_dungeon();
	bool quit_status();
	
private:
	Floor *f;
	bool quit;
	//std::queue<std::string> message_queue;

	//void display_message_queue(int x, int y);
	//void display_monter_list();
	//void scroll_monster_list();
	//void display_primary();
};
#endif

