#ifndef IO_H

#define IO_H

#include <queue>


class IO_handler
{
public:
	IO_handler(Floor *floor);
	~IO_handler();

	void handle_input();
	void display_dungeon();
	
private:
	Floor *f;
	std::queue<std::string> message_queue;

	void display_message_queue(int x, int y);
	void display_monter_list();
	void scroll_monster_list();
	void display_map();
};
#endif

