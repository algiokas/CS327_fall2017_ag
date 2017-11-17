#ifndef PC_H

#define PC_H

#include "Character.h"
#include "Floor.h"

enum equipment_slots {
	WEAPON_SLOT, OFFHAND_SLOT, RANGED_SLOT, ARMOR_SLOT, HELMET_SLOT, CLOAK_SLOT, 
	GLOVES_SLOT, BOOTS_SLOT, AMULET_SLOT, LIGHT_SLOT, RING_SLOT_A, RING_SLOT_B,
	//add new equipment slots here
	NUM_EQUIPMENT_SLOTS
};

const int pc_speed = 10;
const int pc_vision_range = 3;
const int pc_max_hitpoints = 100;
const Dice pc_base_damage(0, 1, 4);
const int pc_inventory_size = 10;

class Object;

class PC : public Character
{
public:
	static PC *instance();
	static void reset();
	
	void update_vision(Floor *f);

	unsigned int roll_damage() override;
	void take_damage(int amount) override;
	unsigned int get_speed() override;

	void do_turn() override;

	bool pick_up_object(Object * obj);
	bool equip_object(int slot);
	bool unequip_object(equipment_slots slot);
	bool drop_item(int slot);
	void expunge_item(int slot);
	Object *inventory_object(int slot);
	Object *equipped_item(equipment_slots slot);

	void tgm();

private:
	static PC *s_instance;

	std::array<Object *, pc_inventory_size> inventory;
	std::array<Object *, NUM_EQUIPMENT_SLOTS> equipment;

	PC() : Character('@', pc_speed, pc_max_hitpoints, pc_base_damage)
	{
		this->vision_range = pc_vision_range;
	}
	~PC() {}

	int vision_range;
	bool god_mode;
};

#endif

