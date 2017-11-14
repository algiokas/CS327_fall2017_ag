#ifndef PC_H

#define PC_H

#include "Character.h"
#include "Floor.h"

const int pc_speed = 10;
const int pc_vision_range = 3;

class PC : public Character
{
public:
	PC();
	PC(int x, int y);
	~PC();
	
	void move_to(int x, int y);
	void update_vision();
	bool can_see(int x, int y);
	bool has_seen(int x, int y);

private:
	std::array<bool, FWIDTH * FHEIGHT> current_vision;
	std::array<bool, FWIDTH * FHEIGHT> known_terrain;
	int vision_range;
};

#endif

