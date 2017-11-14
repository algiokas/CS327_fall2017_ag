#ifndef NPC_H

#define NPC_H

#include "Dice.h"
#include "Character.h"
#include "Definitions.h"

#include <bitset>
#include <string>
#include <vector>

const int npc_vision_range = 15;

class NPC :
	public Character
{
public:
    NPC();
	NPC(std::string name, char symbol, std::string description, int color,
		int speed, unsigned long abilities, int hitpoints, Dice damage) :
		Character(symbol, speed), 
		name(name), 
		description(description),
        color(color),
		ability(abilities), 
        hitpoints(hitpoints), 
        damage(damage) 
        {}
	~NPC();

	inline std::string get_name() { return this->name; }
	inline std::string get_description() { return this->description; }
	inline int get_color() { return this->color; }
	inline int get_speed() { return this->spd; }
	inline bool check_ability(abilities_index a) { return this->ability.test(a); }
	inline int get_hp() { return this->hitpoints; }
	inline int roll_damage() { return this->damage.roll(); }

private:
	std::string name;
	std::string description;
	int color;
	std::bitset<8> ability;
	int hitpoints;
	Dice damage;
};

#endif
