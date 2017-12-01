
#ifndef DEFINITIONS_H

#define DEFINITIONS_H

#include <string>
#include <vector>
#include <bitset>

#include "Dice.h"

enum def_monster_parameters {
	NAME_M, SYMB_M, DESC_M, COLOR_M, SPEED_M, ABIL_M, HP_M, DAM_M, //ADD NEW PARAMETERS HERE
	NUM_PARAMETERS_M
};

enum def_object_parameters {
	NAME_O, DESC_O, TYPE_O, COLOR_O, HIT_O, DAM_O, DODGE_O, DEF_O, WEIGHT_O, SPEED_O, ATTR_O, VAL_O,
	NUM_PARAMETERS_O
};

enum abilities_index {
	SMART, TELE, TUNNEL, ERRATIC, PASS, PICKUP, DESTROY, UNIQ, //ADD NEW ABILTIES HERE
	NUM_ABILITIES
};

enum type_index {
	NO_TYPE, WEAPON, OFFHAND, RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS, AMULET, LIGHT, RING,  //EQUIPMENT IN THIS ROW
	SCROLL, BOOK, FLASK, GOLD, AMMUNITION, FOOD, WAND, CONTAINER //NON-EQUIPMENT IN THIS ROW
};

extern std::vector<std::string> abilities;

extern std::vector<std::string> colors;

extern std::vector<std::string> item_types;

extern const char object_symbol[];

const std::string MONSTER_ENTRY_TOKEN = "BEGIN MONSTER";
const std::string MONSTER_FILE_TOKEN = "RLG327 MONSTER DESCRIPTION 1";
const std::string MONSTERS_FNAME = "monster_desc.txt";

const std::string OBJECT_ENTRY_TOKEN = "BEGIN OBJECT";
const std::string OBJECT_FILE_TOKEN = "RLG327 OBJECT DESCRIPTION 1";
const std::string OBJECTS_FNAME = "object_desc.txt";

std::vector<std::string> split(const std::string &s, char delim);

class NPC;
class Object;

class Monster_definition {
public:
	Monster_definition() :
		name(""), symbol(0), description(""),  color(),
		speed(), ability(), hitpoints(), damage(), times_generated(0)
	{}

	inline void set_name(std::string name) { this->name = name; }
	inline void set_symb(char symbol) { this->symbol = symbol; }
	inline void set_desc(std::string description) { this->description = description; }	
	inline void set_speed(Dice speed) { this->speed = speed; }
	inline void set_hp(Dice hitpoints) { this->hitpoints = hitpoints; }
	inline void set_damage(Dice damage) { this->damage = damage; }

	bool add_color(std::string color);
    void reset_color();
	bool give_ability(std::string ability);

	bool has_ability(abilities_index a);

	void print_def();

	NPC *generate_NPC();
	inline int num_generated() { return this->times_generated; }

	

private:
	std::string name;
	char symbol;
	std::string description;
	std::vector<int> color;
	Dice speed;
	std::bitset<8> ability;
	Dice hitpoints;
	Dice damage;

	int times_generated;
};

class Object_definition {
public:
	Object_definition() :
		name(""), description(""), type(NO_TYPE), is_equipment(true), color(0), 
		hit(), dam(), dodge(), def(), weight(), speed(), attr(), val(), times_generated(0)
	{}

	inline void set_name(std::string name) { this->name = name; }
	inline void set_desc(std::string description) { this->description = description; }
	
	inline void set_hit(Dice hit) { this->hit = hit; }
	inline void set_dam(Dice dam) { this->dam = dam; }
	inline void set_dodge(Dice dodge) { this->dodge = dodge; }
	inline void set_def(Dice def) { this->def = def; }
	inline void set_weight(Dice weight) { this->weight = weight; }
	inline void set_speed(Dice speed) { this->speed = speed; }
	inline void set_attr(Dice attr) { this->attr = attr; }
	inline void set_val(Dice val) { this->val = val; }

	bool set_type(std::string type);
	bool set_color(std::string color);

	void print_def();

	Object *generate_object();


private:
	std::string name;
	std::string description;
	type_index type;
	bool is_equipment;
	int color;
	Dice hit;
	Dice dam;
	Dice dodge;
	Dice def;
	Dice weight;
	Dice speed;
	Dice attr;
	Dice val;

	int times_generated;
};

std::vector<Monster_definition> parse_all_monsters();

std::vector<Object_definition> parse_all_objects();

#endif

