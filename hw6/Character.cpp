#include "Character.h"



Character::Character(char sym, int pos, int spd, int sqnum)
{
	this->symbol = sym;
	this->position = pos;
	this->speed = spd;
	this->alive = true;
	this->sequence_num = sqnum;
}

char Character::symbol()
{
	return 0;
}

int Character::position()
{
	return this->position;
}

int Character::speed()
{
	return this->speed;
}

bool Character::is_alive()
{
	return this->alive;
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
