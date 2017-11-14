#ifndef OBJECT_H

#define OBJECT_H

#include <string>

#include "Definitions.h"

class Object
{
public:
	Object();
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

/*
inline std::string get_name() { return this->name; }
inline std::string get_description() { return this->description; }
inline type_index get_type() { return this->type; }
inline bool can_equip() { return this->is_equipment; }
inline int get_color() { return this->color; }
inline int get_hit_bonus() { return this->hit; }
inline int get_dodge() { return this->dodge; }
inline int get_defense() { return this->defense; }
inline int get_weight() { return this->weight; }
inline int get_speed() { return this->speed; }
inline int get_attribute() { return this->attribute; }
inline int get_value() { return this->value; }

inline int roll_damage() { return this->damage.roll(); }
*/

#endif

