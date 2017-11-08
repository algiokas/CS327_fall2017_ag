#include "PC.h"


PC::PC()
{
	sym = '@';
	x_pos = 0;
	y_pos = 0;
	spd = pc_speed;
	alive = true;
	seq_num = 0;
	vision_range = pc_vision_range;
}

PC::PC(int x, int y)
{
	sym = '@';
	x_pos = x;
	y_pos = y;
	spd = pc_speed;
	alive = true;
	seq_num = 0;
	vision_range = pc_vision_range;
}

PC::~PC()
{
}
