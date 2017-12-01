#ifndef OBJECT_H

#define OBJECT_H

#include <string>

#include "Definitions.h"

class Object
{
public:
	Object(std::string name, std::string desc, type_index type,
           bool is_equip, int color, int hit, Dice dmg, int dodge,
           int def, int weight, int spd, int attr, int val) :
           name(name),
           description(desc),
           type(type),
           is_equipment(is_equip),
           color(color),
           hit(hit),
           damage(dmg),
           dodge(dodge),
           defense(def),
           weight(weight),
           speed(spd),
           attribute(attr),
           value(val) {}
	~Object();

	int roll_damage();

	std::string name;
	std::string description;
	type_index type;
	bool is_equipment;
	int color;
	int hit;
	Dice damage;
	int dodge;
	int defense;
	int weight;
	int speed;
	int attribute;
	int value;	
};

#endif

