#ifndef DUNGEON_H

#define DUNGEON_H

#include <vector>

#include "PC.h"

class Floor;
class Monster_definition;
class Object_definition;

enum direction;
enum isTunneling;

class Dungeon
{
public:
	Dungeon();
	Dungeon(std::string filename);
	~Dungeon();

	void spawn_pc();
	bool move_pc(direction dir);
	bool change_floor_down();
	bool change_floor_up();

	CType get_type(int x, int y);
	int get_hardn(int x, int y);
	int get_dist(int x, int y);
	int get_dist_tunnel(int x, int y);
	Character *get_character(int x, int y);
    Object *get_object(int x, int y);
	duo get_pc_location();

	int get_width();
	int get_height();

	duo rand_room_location();
	void print_floor();

	void save_to_file();
	void save_to_file(std::string filename);

	bool pc_can_see(int x, int y);
	bool pc_has_seen(int x, int y);

private:
	PC pc;
	std::vector<Floor *> floors;
	unsigned int active_floor;
	std::vector<Monster_definition> monster_defs;
	std::vector<Object_definition> object_defs;

	void generate_monsters();
    void generate_objects();



};

#endif
