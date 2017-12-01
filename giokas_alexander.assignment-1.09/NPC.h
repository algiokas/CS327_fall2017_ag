#ifndef NPC_H

#define NPC_H

#include "Dice.h"
#include "Character.h"
#include "Definitions.h"

#include <bitset>
#include <string>
#include <vector>

const int npc_vision_range = 15;

struct pair;

class NPC :
	public Character
{
public:
	NPC(std::string name, char symbol, std::string description, int color,
		int speed, unsigned long abilities, int hitpoints, Dice damage) :
		Character(symbol, speed, hitpoints, damage),
		name(name),
		description(description),
		color(color),
		ability(abilities),
		pc_last_known_position(0)
        {}

	~NPC();

	inline std::string get_name() { return this->name; }
	inline std::string get_description() { return this->description; }
	inline int get_color() { return this->color; }
	inline bool check_ability(abilities_index a) { return this->ability.test(a); }

	int get_next_location(Floor *f);

private:
	std::string name;
	std::string description;
	int color;
	std::bitset<8> ability;

	int pc_last_known_position;

	int smart_next(Floor *f);
	int erratic_next(Floor *f);
	int tele_next(Floor *f);
	int tunnel_next(Floor *f);
	int pass_next(Floor *f);
};

#endif

