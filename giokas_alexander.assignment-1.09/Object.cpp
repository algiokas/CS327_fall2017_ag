#include "Object.h"

Object::~Object()
{
}

int Object::roll_damage()
{
	return this->damage.roll();
}
