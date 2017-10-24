#ifndef DUNGEON_H

#define DUNGEON_H

#include "Character.h"

#include <string>
#include <vector>



//Floor dimensions
#define FWIDTH  80 //floor width
#define FHEIGHT 21 //floor height

//Room generation values
#define MINROOMS  5 //minimum rooms per floor as per assignment spec
#define MAXROOMS  15 //arbitrary upper bound 
#define MINROOMWIDTH 3 //minimum width of a room
#define MINROOMHEIGHT 2 //minimum height of a room
#define ROOMDENSITY 0.12 //the fraction of the floor that must be rooms

//File IO values
#define RPATH "/.rlg327/" //the relative path from the home directory to the save/load file
#define DEFAULT_FNAME "dungeon"
#define FMARKER "RLG327"
#define PVERSION 0 //the program version

//converts the 2D representation of a location into a linear array index
#define INDEX2D(x, y) (FWIDTH * y) + x

//converts a linear array index into either the x or y of its 2D representation
#define LINEARX(idx) (idx % FWIDTH)
#define LINEARY(idx) (idx / FWIDTH)

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

std::array<int, 8> get_neighbors(int x, int y);

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
	void draw_path(int len, int* path);
	void add_corridors();

	void load_from_file();
	void load_from_file(std::string filename);

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

	struct duo rand_room_location();
	void print_floor();

	void save_to_file();
	void save_to_file(std::string filename);
};



#endif

