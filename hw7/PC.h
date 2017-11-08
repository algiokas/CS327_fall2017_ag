#ifndef PC_H

#define PC_H

#include "Character.h"

const int pc_speed = 10;
const int pc_vision_range = 3;

class PC : public Character
{
public:
	PC();
	PC(int x, int y);
	~PC();
	
	//duo next_position();

private:
	//bool current_vision[FWIDTH * FHEIGHT];
	//bool known_terrain[FWIDTH * FHEIGHT];
	int vision_range;
};

#endif

