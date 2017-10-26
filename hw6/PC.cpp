#include "PC.h"


PC::PC(int x, int y)
{
	this->symbol = '@';
	this->speed = pc_speed;
	this->is_alive = true;
	this->seq_num = 0;
}

PC::~PC()
{
}

void PC::attempt_move()
{
}
