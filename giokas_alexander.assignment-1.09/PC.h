#ifndef PC_H

#define PC_H

#include "Character.h"
#include "Floor.h"

#include <cmath>

const int pc_speed = 10;
const int pc_vision_range = 3;
const int pc_hitpoints = 100;
const Dice pc_base_damage(0, 1, 4);


class PC : public Character
{
public:
	PC() : Character('@', pc_speed, pc_hitpoints, pc_base_damage)
	{
		this->vision_range = pow(pc_vision_range, 2);
		current_vision.fill(false);
		known_terrain.fill(false);
	}

	PC(int x, int y) : Character('@', pc_speed, pc_hitpoints, pc_base_damage)
	{
		set_location(x, y);
		this->vision_range = pow(pc_vision_range, 2);
		current_vision.fill(false);
		known_terrain.fill(false);
	}
	~PC();
	
	void update_vision();
	bool can_see(int x, int y);
	bool has_seen(int x, int y);
    void reset_vision();

private:
	std::array<bool, FWIDTH * FHEIGHT> current_vision;
	std::array<bool, FWIDTH * FHEIGHT> known_terrain;
	int vision_range;
};

#endif

