#ifndef DUNGEON_H

#define DUNGEON_H

#include <vector>

#include "Floor.h"
#include "PQueue.h"

const std::string BOSS_NAME = "SpongeBob SquarePants";

class Monster_definition;
class Object_definition;
class NPC;

class Dungeon
{
public:
	static Dungeon *instance();
	static void reset();

	static void init();
	static void init(std::string filename);

	void spawn_pc();
	bool move_pc(direction dir);
	bool move_npc(NPC *npc, int next_location);
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

	bool place_object(int x, int y, Object *obj);

	int total_monsters();
	void send_to_graveyard(Character *c);
	Character *boss_ptr;

	void add_event(int time, Character * c);
	void clear_event_queue();

	void game_turn();

private:
	static Dungeon *s_instance;

	bool is_init;

	Dungeon();
	~Dungeon();

	std::vector<Floor *> floors;
	unsigned int active_floor;
	std::vector<Monster_definition> monster_defs;
	std::vector<Object_definition> object_defs;

	std::vector<Character *> graveyard;

	void generate_monsters();
    void generate_objects();

	PQueue<Character *> event_queue;

	void queue_monster_turns();
	void queue_pc_turn();


};

#endif
