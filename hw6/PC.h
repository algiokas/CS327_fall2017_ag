#ifndef PC_H

#define PC_H

#include "Character.h"

const int pc_speed = 10;
const int pc_vision_range = 3;

class PC : public Character
{
private:
	bool current_vision[FWIDTH * FHEIGHT];
	bool known_terrain[FWIDTH * FHEIGHT];
	int vision_range;

public:
	PC(int x, int y);
	~PC();



};

#endif

