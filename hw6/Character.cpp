#include "Character.h"



Character::Character(char sym, int x, int y, int spd, int sqnum)
{
	this->symbol = sym;
	x_pos = x;
	y_pos = y;
	this->speed = spd;
	alive = true;
	this->sequence_num = sqnum;
}

Character::~Character()
{
}

char Character::symbol()
{
	return this->symbol;
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
	return this->speed;
}

bool Character::is_alive()
{
	return alive;
}

int Character::sequence_num()
{
	return this->sequence_num;
}

bool Character::can_see(Floor * f, Character c)
{
	return false;
}

bool Character::attempt_move()
{
	return false;
}

void Character::kill()
{
	alive = false;
}
