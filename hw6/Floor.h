#ifndef DUNGEON_H

#define DUNGEON_H

#include "Character.h"

#include <string>
#include <vector>
#include <array>

//Floor dimensions
const int FWIDTH = 80; //floor width
const int FHEIGHT = 21; //floor height

//Room generation values
const int MINROOMS = 5; //minimum rooms per floor as per assignment spec
const int MAXROOMS = 15; //arbitrary upper bound 
const int MINROOMWIDTH = 3; //minimum width of a room
const int MINROOMHEIGHT = 2; //minimum height of a room
const double ROOMDENSITY = 0.12; //the fraction of the floor that must be rooms

//File IO values
const std::string RPATH = "/.rlg327/"; //the relative path from the home directory to the save/load file
const std::string DEFAULT_FNAME = "dungeon";
const std::string FMARKER = "RLG327";
const int PVERSION = 0; //the program version

/* 	HMODVAL is a constant that is added to the hardness value of rooms
	and corridors during corridor generation which essentially adds an
	artificial bias of the alorithm against generating corridor cells in
	rooms. Without this value (or with it set to 0), dijkstra_corridor()
	will take a path that spends as little time carving out rock as possible.
	However the higher the HMODVAL, the more attractive rock is compared to
	rooms and corridors. This tends to cause the generated corridors to
	meander more and creates more interesting dungeons. */
	const int HMODVAL = 100;

typedef enum cellType {
	error_c,
	immutable_c,
	rock_c,
	room_c,
	corridor_c,
	stairs_up_c,
	stairs_down_c
} CType;

struct duo {
	int x;
	int y;
};

struct room {
	struct duo loc;
	struct duo dims;
};

enum isTunneling {
	non_tunneling,
	tunneling
};

//returns an array that contains the indices of the 8 neighbors of a given cell
std::array<int, 8> get_neighbors(int x, int y);

//converts the 2D representation of a location into a linear array index
inline int index2d(int x, int y) { return (FWIDTH * y) + x; }

//converts a linear array index into either the x or y of its 2D representation
inline int linearX(int index) { return (index % FWIDTH); }

inline int linearY(int index) { return (index / FWIDTH); }

class Floor
{
private:
	int width;
	int height;
	int num_rooms;
	int pc_loc;
	int num_monsters;
	int max_monsters;
	int time;

	PC *pc;

	std::vector<struct room> rooms;
	std::array<CType, FWIDTH * FHEIGHT> type_map;
	std::array<int, FWIDTH * FHEIGHT>  hard_map;
	std::array<int, FWIDTH * FHEIGHT> dist_map;
	std::array<int, FWIDTH * FHEIGHT> dist_map_tunnel;
	std::array<Character *, FWIDTH * FHEIGHT> char_map;

	void empty_floor();
	void place_room(struct room *r);
	bool check_intersection(struct room *r);
	void gen_rooms();

	std::vector<int> dijkstra_corridor(int source, int target);
	void draw_path(std::vector<int> path);
	void add_corridors();
	void load_from_file(std::string filename);

	void spawn_pc(int x, int y);

public:
	Floor();
	Floor(std::string filename);
	~Floor();

	void set_cell_type(int x, int y, CType input_type);
	void set_cell_hardn(int x, int y, int input_hardn);
	void set_cell_dist(int x, int y, int dist);
	void set_cell_dist_tunnel(int x, int y, int dist);
	void place_character(int x, int y, Character *c);

	CType get_type(int x, int y);
	int get_hardn(int x, int y);
	int get_dist(int x, int y);
	int get_dist_tunnel(int x, int y);
	Character *get_character(int x, int y);

	duo rand_room_location();
	void spawn_pc();
	void print_floor();


	void save_to_file();
	void save_to_file(std::string filename);

	void update_dist(isTunneling t);
	 
};

#endif

