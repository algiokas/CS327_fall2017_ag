#include "Character.h"



Character::Character(char sym, int x, int y, int spd, int sqnum)
{
	this->sym = sym;
	this->x_pos = x;
	this->y_pos = y;
	this->spd = spd;
	this->alive = true;
	this->seq_num = sqnum;
}

Character::~Character()
{
}

char Character::symbol()
{
	return sym;
}

int Character::x()
{
	return x_pos;
}

int Character::y()
{
	return y_pos;
}


int Character::speed()
{
	return spd;
}

bool Character::is_alive()
{
	return alive;
}

int Character::sequence_num()
{
	return seq_num;
}

void Character::kill()
{
	alive = false;
}

void Character::set_location(int x, int y)
{
    this->x_pos = x;
    this->y_pos = y;
}
