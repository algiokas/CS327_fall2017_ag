
#ifndef DEFINITIONS_H

#define DEFINITIONS_H

#include <string>
#include <vector>
#include <bitset>

#include "Dice.h"

enum def_parameters {
	NAME, SYMB, DESC, COLOR, SPEED, ABIL, HP, DAM, //ADD NEW PARAMETERS HERE
	NUM_PARAMETERS
};

enum abilities_index {
	SMART, TELE, TUNNEL, ERRATIC, PASS, PICKUP, DESTROY, UNIQ, //ADD NEW ABILTIES HERE
	NUM_ABILITIES
};

extern std::vector<std::string> abilities;

extern std::vector<std::string> colors;

const std::string MONSTER_ENTRY_TOKEN = "BEGIN MONSTER";
const std::string MONSTER_FILE_TOKEN = "RLG327 MONSTER DESCRIPTION 1";
const std::string MONSTERS_FNAME = "monster_desc.txt";

class Dice;

class Monster_definition {
public:
	Monster_definition() :
		name(""), symbol(0), description(""),  color(),
		speed(), ability(), hitpoints(), damage() {}

	inline void set_name(std::string name) { this->name = name; }
	inline void set_symb(char symbol) { this->symbol = symbol; }
	inline void set_desc(std::string description) { this->description = description; }	
	inline void set_speed(Dice speed) { this->speed = speed; }
	inline void set_hp(Dice hitpoints) { this->hitpoints = hitpoints; }
	inline void set_damage(Dice damage) { this->damage = damage; }

	bool add_color(std::string color);
	bool give_ability(std::string ability);

	void print_def();

private:
	std::string name;
	char symbol;
	std::string description;
	std::vector<int> color;
	Dice speed;
	std::bitset<8> ability;
	Dice hitpoints;
	Dice damage;
};

std::vector<Monster_definition> parse_all_monsters();

#endif

