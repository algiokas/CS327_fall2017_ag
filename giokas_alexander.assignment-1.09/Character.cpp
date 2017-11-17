#include "Character.h"

Character::Character()
{
	this->sym = 0;
	this->position.x = 0;
	this->position.y = 0;
	this->spd = 0;
	this->seq_num = 0;
	this->hp = 0;
	this->damage = Dice(0, 0, 0);
}

Character::~Character()
{
}

void Character::set_location(int x, int y)
{
	this->position.x = x;
	this->position.y = y;
}

unsigned int Character::roll_damage()
{
	return this->damage.roll();
}

void Character::take_damage(int amount)
{
	this->hp = hp - amount;
}

unsigned int Character::get_speed()
{
	return this->spd;
}

void Character::do_turn()
{
	//this should never be called
}
