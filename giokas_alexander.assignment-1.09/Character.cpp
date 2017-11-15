#include "Character.h"





Character::~Character()
{
}

void Character::set_location(int x, int y)
{
	this->position.x = x;
	this->position.y = y;
}

int Character::roll_damage()
{
	return this->damage.roll();
}

void Character::take_damage(int amount)
{
	this->hp = hp - amount;
}
