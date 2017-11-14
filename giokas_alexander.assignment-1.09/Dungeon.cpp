#include "Dungeon.h"
#include "Floor.h"
#include "Definitions.h"

#include <iostream>


Dungeon::Dungeon()
{
	std::cout << "Constructing a Dungeon" << std::endl;
	Floor *f = new Floor();
	this->floors.push_back(f);
	this->active_floor = 0;
	generate_monsters();
	duo spawn_point = rand_room_location();
	pc = PC(spawn_point.x, spawn_point.y);
	spawn_pc();

	monster_defs = parse_all_monsters();
	object_defs = parse_all_objects();
}

Dungeon::Dungeon(std::string filename)
{
	std::cout << "Constructing a Dungeon from file" << std::endl;
	Floor *f = new Floor(filename);
	this->floors.push_back(f);
	this->active_floor = 0;
	generate_monsters();
	duo spawn_point = rand_room_location();
	pc = PC(spawn_point.x, spawn_point.y);
	spawn_pc();

	monster_defs = parse_all_monsters();
	object_defs = parse_all_objects();
}


Dungeon::~Dungeon()
{
	for (Floor *f : this->floors) {
		delete f;
	}
}

void Dungeon::spawn_pc()
{
	floors.at(active_floor)->spawn_pc(&this->pc);
}

bool Dungeon::move_pc(direction dir)
{
	return floors.at(active_floor)->move_pc(dir);
}

bool Dungeon::change_floor_down()
{
	if (floors.size() <= active_floor+1) {
		Floor *f = new Floor();
		f->spawn_pc(&pc);
		f->place_at_stairs(true);
		this->floors.push_back(f);
	}
	this->active_floor++;
	generate_monsters();
	return true;
}

bool Dungeon::change_floor_up()
{
	active_floor--;
	return true;
}

CType Dungeon::get_type(int x, int y)
{
	return this->floors.at(active_floor)->get_type(x, y);
}

int Dungeon::get_hardn(int x, int y)
{
	return this->floors.at(active_floor)->get_hardn(x, y);
}

int Dungeon::get_dist(int x, int y)
{
	return this->floors.at(active_floor)->get_dist(x, y);
}

int Dungeon::get_dist_tunnel(int x, int y)
{
	return this->floors.at(active_floor)->get_dist_tunnel(x, y);
}

Character * Dungeon::get_character(int x, int y)
{
	return this->floors.at(active_floor)->get_character(x, y);
}

duo Dungeon::get_pc_location()
{
	return this->floors.at(active_floor)->get_pc_location();
}

int Dungeon::get_width()
{
	return this->floors.at(active_floor)->get_width();
}

int Dungeon::get_height()
{
	return this->floors.at(active_floor)->get_height();
}

duo Dungeon::rand_room_location()
{
	return this->floors.at(active_floor)->rand_room_location();
}

void Dungeon::print_floor()
{
	this->floors.at(active_floor)->print_floor();
}

void Dungeon::save_to_file()
{
	this->floors.at(active_floor)->save_to_file();
}

void Dungeon::save_to_file(std::string filename)
{
	this->floors.at(active_floor)->save_to_file(filename);
}


bool Dungeon::pc_can_see(int x, int y)
{
	return this->floors.at(active_floor)->pc_can_see(x, y);
}

bool Dungeon::pc_has_seen(int x, int y)
{
	return this->floors.at(active_floor)->pc_has_seen(x, y);
}

void Dungeon::generate_monsters()
{
	int num_defs = monster_defs.size();
	int mon;
	duo loc;
	for (int i = 0; i < 10; i++) {
		mon = rand() % num_defs;
		loc = this->floors.at(active_floor)->rand_room_location();

		while (monster_defs.at(mon).has_ability(UNIQ) || monster_defs.at(mon).num_generated() > 0) {
			mon = rand() % num_defs;
		}
		while (this->floors.at(active_floor)->get_character(loc.x, loc.y)) {
			loc = this->floors.at(active_floor)->rand_room_location();
		}
		this->floors.at(active_floor)->place_character(loc.x, loc.y,
			((Character *) monster_defs.at(mon).generate_NPC()));
	}
}
