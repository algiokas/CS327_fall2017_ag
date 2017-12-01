#include "Dungeon.h"
#include "Definitions.h"
#include "PQueue.h"
#include "PC.h"
#include "IOhandler.h"
#include "NPC.h"

#include <iostream>

Dungeon *Dungeon::s_instance = 0;

Dungeon::Dungeon()
{
}

Dungeon::~Dungeon()
{
	for (Floor *f : s_instance->floors) {
		delete f;
	}
	for (Character *c : graveyard) {
		delete c;
	}
	clear_event_queue();
}

Dungeon *Dungeon::instance()
{
	if (!s_instance) {
		s_instance = new Dungeon();
	}
	return s_instance;
}

void Dungeon::reset()
{
	if (s_instance) {
		delete s_instance;
	}
}

//STATIC
void Dungeon::init()
{
	if (s_instance->is_init) {
		std::cout << "Dungeon already initialized..." << std::endl;
		return;
	}
	std::cout << "Generating new Dungeon..." << std::endl;
	Floor *f = new Floor();
	s_instance->floors.push_back(f);
	s_instance->active_floor = 0;
	duo spawn_point = s_instance->rand_room_location();
	PC::instance()->set_location(spawn_point.x, spawn_point.y);
	PC::instance()->update_vision(s_instance->floors.at(s_instance->active_floor));
	s_instance->spawn_pc();

	s_instance->monster_defs = parse_all_monsters();
	s_instance->object_defs = parse_all_objects();

	s_instance->generate_monsters();
	s_instance->generate_objects();

	s_instance->is_init = true;

	std::cout << "Dungeon generation complete" << std::endl;

	IO_handler::instance()->queue_message("Welcome to the dungeon");
}

//STATIC
void Dungeon::init(std::string filename)
{
	if (s_instance->is_init) {
		std::cout << "Dungeon already initialized..." << std::endl;
		return;
	}
	std::cout << "Loading Dungeon from file..." << std::endl;
	Floor *f = new Floor(filename);
	s_instance->floors.push_back(f);
	s_instance->active_floor = 0;
	duo spawn_point = s_instance->rand_room_location();
	PC::instance()->set_location(spawn_point.x, spawn_point.y);
	PC::instance()->update_vision(s_instance->floors.at(s_instance->active_floor));
	s_instance->spawn_pc();

	s_instance->monster_defs = parse_all_monsters();
	s_instance->object_defs = parse_all_objects();

	s_instance->generate_monsters();
	s_instance->generate_objects();

	s_instance->is_init = true;

	std::cout << "Dungeon generation complete" << std::endl;

	IO_handler::instance()->queue_message("Welcome to the dungeon");
}

void Dungeon::spawn_pc()
{
	floors.at(active_floor)->spawn_pc(PC::instance());
}

bool Dungeon::move_pc(direction dir)
{
	return floors.at(active_floor)->move_pc(dir);
}

bool Dungeon::move_npc(NPC * npc, int next_location)
{
	return false;
}

bool Dungeon::change_floor_down()
{
	clear_event_queue();
	if (floors.size() <= active_floor+1) {
		Floor *f = new Floor();
		f->spawn_pc(PC::instance());
		f->place_at_stairs(true);
		s_instance->floors.push_back(f);

		s_instance->active_floor++;
		s_instance->generate_monsters();
		s_instance->generate_objects();
		s_instance->active_floor--;
	}
	s_instance->floors.at(active_floor)->reset_current_vision();
	s_instance->active_floor++;
	queue_monster_turns();
	PC::instance()->update_vision(floors.at(active_floor));
	return true;
}

bool Dungeon::change_floor_up()
{
	if (active_floor == 0) {
		IO_handler::queue_message("The way up is blocked, you are trapped in the dungeon");
		return true;
	}
	clear_event_queue();
	s_instance->floors.at(active_floor)->reset_current_vision();
	active_floor--;
	queue_monster_turns();
	PC::instance()->update_vision(floors.at(active_floor));
	return true;
}

CType Dungeon::get_type(int x, int y)
{
	return s_instance->floors.at(active_floor)->get_type(x, y);
}

int Dungeon::get_hardn(int x, int y)
{
	return s_instance->floors.at(active_floor)->get_hardn(x, y);
}

int Dungeon::get_dist(int x, int y)
{
	return s_instance->floors.at(active_floor)->get_dist(x, y);
}

int Dungeon::get_dist_tunnel(int x, int y)
{
	return s_instance->floors.at(active_floor)->get_dist_tunnel(x, y);
}

Character * Dungeon::get_character(int x, int y)
{
	return s_instance->floors.at(active_floor)->get_character(x, y);
}

duo Dungeon::get_pc_location()
{
	return s_instance->floors.at(active_floor)->get_pc_location();
}

int Dungeon::get_width()
{
	return s_instance->floors.at(active_floor)->get_width();
}

int Dungeon::get_height()
{
	return s_instance->floors.at(active_floor)->get_height();
}

duo Dungeon::rand_room_location()
{
	return s_instance->floors.at(active_floor)->rand_room_location();
}

void Dungeon::print_floor()
{
	s_instance->floors.at(active_floor)->print_floor();
}

void Dungeon::save_to_file()
{
	s_instance->floors.at(active_floor)->save_to_file();
}

void Dungeon::save_to_file(std::string filename)
{
	s_instance->floors.at(active_floor)->save_to_file(filename);
}

bool Dungeon::pc_can_see(int x, int y)
{
	return s_instance->floors.at(active_floor)->can_see(x, y);
}

bool Dungeon::pc_has_seen(int x, int y)
{
	return s_instance->floors.at(active_floor)->has_seen(x, y);
}

bool Dungeon::place_object(int x, int y, Object * obj)
{
	if (!get_object(x, y)) {
		floors.at(active_floor)->place_object(x, y, obj);
		return true;
	}
	std::array<int, 8> n = floors.at(active_floor)->get_neighbors(x, y);
	for (int i : n) {
		if (floors.at(active_floor)->place_object(linearX(i), linearY(i), obj)) {
			return true;
		}
	}
	return false;
}

int Dungeon::total_monsters()
{
	int total = 0;
	for (Floor *f : floors) {
		total += f->get_num_monsters();
	}
	return total;
}

void Dungeon::send_to_graveyard(Character * c)
{
	graveyard.push_back(c);
}

void Dungeon::add_event(int time, Character *c)
{
	this->event_queue.insert(time, c);
}

void Dungeon::clear_event_queue()
{
	while (s_instance->event_queue.peek()) {
		delete s_instance->event_queue.remove_min();
	}
}

void Dungeon::game_turn()
{

	Character *c;
	if (PC::instance()->is_alive()) {
		queue_pc_turn();
	}

	while (PC::instance()->is_alive() &&
		(c = event_queue.remove_shift()) &&
		(c != PC::instance())) {
		int next = ((NPC*)c)->get_next_location(floors.at(active_floor));
		move_npc((NPC *)c, next);

		add_event((1000 / c->get_speed()), c);
	}
	IO_handler::instance()->display_dungeon();
	if (PC::instance()->is_alive() && c == PC::instance()) {
		IO_handler::instance()->handle_input();
	}
}

void Dungeon::generate_monsters()
{
	int num_defs = monster_defs.size();
    std::cout << "Generating and placing monsters..." << std::endl;
    if (num_defs < 1) {
        return;
    }
	int mon;
	duo loc;
	for (int i = 0; i < 10; i++) {
		mon = rand() % num_defs;
		loc = s_instance->floors.at(active_floor)->rand_room_location();

		while (monster_defs.at(mon).has_ability(UNIQ) && monster_defs.at(mon).num_generated() > 0) {
			mon = rand() % num_defs;
		}
		while (s_instance->floors.at(active_floor)->get_character(loc.x, loc.y)) {
			loc = s_instance->floors.at(active_floor)->rand_room_location();
		}
		s_instance->floors.at(active_floor)->place_character(loc.x, loc.y,
			((Character *) monster_defs.at(mon).generate_NPC()));

		Character *cp = get_character(loc.x, loc.y);
		add_event((1000 / cp->get_speed()), cp);
		if (((NPC *)cp)->get_name() == BOSS_NAME) {
			Dungeon::instance()->boss_ptr = cp;
		}
	}
}


void Dungeon::generate_objects()
{
	int num_defs = object_defs.size();
    std::cout << "Generating and placing objects..." << std::endl;
    if (num_defs < 1) {
        return;
    }
	int obj;
	duo loc;
	for (int i = 0; i < 10; i++) {
		obj = rand() % num_defs;
		loc = s_instance->floors.at(active_floor)->rand_room_location();
		while (s_instance->floors.at(active_floor)->get_object(loc.x, loc.y)) {
			loc = s_instance->floors.at(active_floor)->rand_room_location();
		}
		s_instance->floors.at(active_floor)->place_object(loc.x, loc.y,
			(object_defs.at(obj).generate_object()));
	}
}

void Dungeon::queue_monster_turns()
{
	Character *c;
	for (int y = 0; y < s_instance->get_height(); y++) {
		for (int x = 0; x < s_instance->get_width(); x++) {
			if ((c = get_character(x, y))) {
				add_event((1000 / c->get_speed()), c);
			}
		}
	}
}

void Dungeon::queue_pc_turn()
{
	add_event((1000 / PC::instance()->get_speed()), PC::instance());
}

Object * Dungeon::get_object(int x, int y)
{
    return s_instance->floors.at(active_floor)->get_object(x, y);
}
