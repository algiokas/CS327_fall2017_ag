#include "Floor.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <byteswap.h>

std::array<int, 8> get_neighbors(int x, int y)
{
	int index = INDEX2D(x, y);
	int left = index - 1;
	int top = index - FWIDTH;
	int right = index + 1;
	int bottom = index + FWIDTH;
	int topleft = top - 1;
	int topright = top + 1;
	int botright = bottom + 1;
	int botleft = bottom - 1;

	std::array<int, 8> n;

	n[0] = left;
	n[1] = topleft;
	n[2] = top;
	n[3] = topright;
	n[4] = right;
	n[5] = botright;
	n[6] = bottom;
	n[7] = botleft;

	return n;
}

Floor::Floor()
{
}

Floor::Floor(std::string filename)
{
}


Floor::~Floor()
{
}

void Floor::set_cell_type(int x, int y, CType input_type)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	type_map[INDEX2D(x, y)] = input_type;
}

void Floor::set_cell_hardn(int x, int y, int input_hardn)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	if (input_hardn < 0 || input_hardn > 255) {
		throw "Invalid hardness value";
	}
	hard_map[INDEX2D(x, y)] = input_hardn;
}

void Floor::set_cell_dist(int x, int y, int dist)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	dist_map[INDEX2D(x, y)] = dist;
}

void Floor::set_cell_dist_tunnel(int x, int y, int dist)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	dist_map_tunnel[INDEX2D(x, y)] = dist;
}

void Floor::place_character(int x, int y, Character * c)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	char_map[INDEX2D(x, y)] = c;
}

CType Floor::get_type(int x, int y)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	return type_map[INDEX2D(x, y)];
}

int Floor::get_hardn(int x, int y)
{
	if (x < 0 || x >= FWIDTH || y < 0 || y >= FHEIGHT) {
		throw "Cell index out of bounds";
	}
	return hard_map[INDEX2D(x, y)];
}

int Floor::get_dist(int x, int y)
{
	return dist_map[INDEX2D(x, y)];
}

int Floor::get_dist_tunnel(int x, int y)
{
	return dist_map_tunnel[INDEX2D(x, y)];
}

Character * Floor::get_character(int x, int y)
{
	return char_map[INDEX2D(x, y)];;
}

void Floor::empty_floor()
{
	int x, y;
	for (y = 0; y < FHEIGHT; y++) {
		for (x = 0; x < FWIDTH; x++) {
			if (x == 0 || x == FWIDTH - 1 || y == 0 || y == FHEIGHT - 1) {
				set_cell_type(x, y, immutable_c);
				set_cell_hardn(x, y, 255);
			}
			else {
				int rand_hardness = 1 + (std::rand() % 254);
				set_cell_type(x, y, immutable_c);
				set_cell_hardn(x, y, rand_hardness);
			}
		}
	}
}

void Floor::place_room(room * r)
{
	int x, y;
	for (y = r->loc.y; y < r->loc.y + r->dims.y; y++) {
		for (x = r->loc.x; x < r->loc.x + r->dims.x; x++) {
			set_cell_type(x, y, room_c);
			set_cell_hardn(x, y, 0);
		}
	}
}

bool Floor::check_intersection(room * r)
{
	if (this->num_rooms == 0) {
		return false;
	}
	int roomIter;
	for (roomIter = 0; roomIter < num_rooms; roomIter++) {
		//temporary variables assigned for clarity
		int new_left = r->loc.x;
		int new_right = r->loc.x + r->dims.x;
		int new_top = r->loc.y;
		int new_bottom = r->loc.y + r->dims.y;

		int old_left = rooms[roomIter].loc.x;
		int old_right = rooms[roomIter].loc.x + rooms[roomIter].dims.x;
		int old_top = rooms[roomIter].loc.y;
		int old_bottom = rooms[roomIter].loc.y + rooms[roomIter].dims.y;

		bool x_intersect = 0;
		bool y_intersect = 0;

		if (old_left <= new_left && new_left <= old_right) { x_intersect = true; }
		if (old_left <= new_right && new_right <= old_right) { x_intersect = true; }
		if (new_left <= old_left && old_right <= new_right) { x_intersect = true; }
		if (old_top <= new_top && new_top <= old_bottom) { y_intersect = true; }
		if (old_top <= new_bottom && new_bottom <= old_bottom) { y_intersect = true; }
		if (new_top <= old_top && old_bottom <= new_bottom) { y_intersect = true; }

		if (x_intersect && y_intersect) { return true; }
	}
	return false;
}

void Floor::gen_rooms()
{
	double floorsize = width * height;
	double freespace = floorsize; //the total amount of free space within which to place rooms
	int roomIter = 0;
	struct room newRoom; //blank room that we can fill with data and pass to functions

	while ((num_rooms < MINROOMS || (freespace / floorsize) > 1 - ROOMDENSITY)
		&& num_rooms < MAXROOMS) {
		//Rooms should never touch the edge
		newRoom.loc.x = 1 + (std::rand() % (FWIDTH - MINROOMWIDTH - 1));
		newRoom.loc.y = 1 + (std::rand() % (FHEIGHT - MINROOMHEIGHT - 1));

		int maxwidth = FWIDTH - newRoom.loc.x - 1;
		int maxheight = FHEIGHT - newRoom.loc.y - 1;
		if (maxwidth > MINROOMWIDTH) {
			newRoom.dims.x = MINROOMWIDTH + (std::rand() % (maxwidth - MINROOMWIDTH));
		}
		else {
			newRoom.dims.x = MINROOMWIDTH;
		}

		if (maxheight > MINROOMWIDTH) {
			newRoom.dims.y = MINROOMHEIGHT + (std::rand() % (maxheight - MINROOMHEIGHT));
		}
		else {
			newRoom.dims.y = MINROOMHEIGHT;
		}

		if (!check_intersection(&newRoom)) {
			rooms[roomIter] = newRoom;
			num_rooms++;
			place_room(&newRoom);
			//get the total space taken up by the new room
			int roomsize = newRoom.dims.x * newRoom.dims.y;
			freespace -= roomsize; //and subtract it from free space
			roomIter++;
		}
	}
}

duo Floor::rand_room_location()
{
	int r, x, y;
	r = std::rand() % num_rooms;

	struct duo out;
	out.x = rooms[r].loc.x + (std::rand() % rooms[r].loc.y);
	out.y = rooms[r].loc.y + (std::rand() % rooms[r].loc.y);

	return out;
}

void Floor::draw_path(int len, int * path)
{
	if (!path) {
		throw "invalid draw path";
	}
	int i = 0;
	//while(path[i]) {
	while (i < len) {
		int pathx = (LINEARX(path[i]));
		int pathy = (LINEARY(path[i]));
		if (get_type(pathx, pathy) == rock_c) {
			set_cell_type(pathx, pathy, corridor_c);
			set_cell_hardn(pathx, pathy, 0);
		}
		i++;
	}
}

void Floor::add_corridors()
{
}

void Floor::load_from_file()
{
	load_from_file(DEFAULT_FNAME);
}

void Floor::load_from_file(std::string filename)
{

	std::string fpath;
	std::ifstream f;
	std::string marker = FMARKER;
	int i, ch;
	int32_t version = PVERSION;

	width = FWIDTH;
	height = FHEIGHT;
	num_rooms = 0;
	pc_loc = 0;

	fpath = std::string(std::getenv("HOME"));
	if (fpath.empty()) {
		fpath = ".";
	}
	fpath.append(RPATH);
	fpath.append(filename);

	unsigned char marker_buff[7];
	uint8_t version_buff[4];
	uint8_t size_buff[4];

	struct room temp_room;

	f.open(fpath);

	while (f >> std::noskipws >> ch) {
		//handle file marker
		if (i < 6) {
			marker_buff[i] = (unsigned char)ch;
		}
		else if (i == 6) {
			marker_buff[6] = '\0';
			std::string marker_str = reinterpret_cast<char*>(marker_buff);
			if (!marker.compare(marker_str))
				throw "Input file has invalid marker";
		}

		//handle version number
		if (i > 5 && i < 10) {
			version_buff[9 - i] = (uint8_t)ch; //place bytes in reverse order 
		}
		else if (i == 10) {
			int v = (uint32_t)version_buff[0] << 24 |
				(uint32_t)version_buff[1] << 16 |
				(uint32_t)version_buff[2] << 8 |
				(uint32_t)version_buff[3];
			if (v != version) {
				throw "input file has invalid version";
			}
		}

		//handle file size
		if (i > 9 && i < 14) {
			size_buff[13 - i] = (uint8_t)ch; //place bytes in reverse order 
		}
		else if (i == 14) {
			size_buff[4] = '\0';
			int s = (uint32_t)size_buff[0] << 24 |
				(uint32_t)size_buff[1] << 16 |
				(uint32_t)size_buff[2] << 8 |
				(uint32_t)size_buff[3];
			//fsize = s;
			if (s < 1694) {
				throw "input file has invalid size";
			}
		}

		//read dungeon cells
		if (i > 13 && i < 1694) {
			hard_map[i - 14] = ch;
			if (ch == 255) {
				type_map[i - 14] = immutable_c;
			}
			else if (ch == 0) {
				type_map[i - 14] = corridor_c;
			}
			else {
				type_map[i - 14] = rock_c;
			}
		}
		if (i > 1693) {
			switch ((i - 1694) % 4) {
			case 0:
				temp_room.loc.y = ch;
				break;
			case 1:
				temp_room.loc.x = ch;
				break;
			case 2:
				temp_room.dims.y = ch;
				break;
			case 3:
				temp_room.dims.x = ch;
				int roomNum = (i - 1694) / 4;
				rooms[roomNum] = temp_room;
				num_rooms++;
				place_room(&temp_room);
			}
		}
		i++;
	}
}

void Floor::save_to_file()
{
	save_to_file(DEFAULT_FNAME);
}

void Floor::save_to_file(std::string filename)
{
	std::string fpath;
	std::ofstream f;
	std::string marker = FMARKER;
	int i, ch;
	int32_t version = PVERSION;
	int32_t fsize;

	fpath = std::string(std::getenv("HOME"));
	if (fpath.empty()) {
		fpath = ".";
	}
	fpath.append(RPATH);
	fpath.append(filename);

	int headersize = marker.length() + (sizeof version) + (sizeof fsize);
	fsize = headersize + (FWIDTH * FHEIGHT) + (4 * num_rooms);

	f.open(fpath, std::ios_base::out | std::ios_base::binary);

	//begin file write
	f.write(marker.c_str(), marker.length());
	f.write(bswap_32(version), sizeof version);
	f.write(bswap_32(fsize), sizeof fsize);

	int i;
	for (i = 0; i < FWIDTH * FHEIGHT; i++) {
		int x = (i % FWIDTH);
		int y = (i / FWIDTH);
		f.put(get_hardn(x, y));
	}

	int room;
	for (room = 0; room < num_rooms; room++) {
		f.put(rooms[room].loc.y);
		f.put(rooms[room].loc.x);
		f.put(rooms[room].dims.y);
		f.put(rooms[room].dims.x);
	}
	f.close();
}

void Floor::print_floor()
{
	int row, col, dash;
	std::cout << std::endl << "Dungeon Map" << std::endl;
	//top edge of border
	for (dash = 0; dash < FWIDTH + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
	for (col = 0; col < FHEIGHT; col++) {
		std::cout << "|";
		for (row = 0; row < FWIDTH; row++) {
			if (INDEX2D(row, col) == pc_loc) {
				std::cout << "@";
			}
			else {
				switch (get_type(row, col)) {
				case(immutable_c):
					std::cout << " ";
					break;
				case(rock_c):
					std::cout << " ";
					break;
				case(room_c):
					std::cout << ".";
					break;
				case(corridor_c):
					std::cout << "#";
					break;
				default:
					std::cout << "X"; //indicates an error
				}
			}
		}
		std::cout << std::endl;
	}
	//bottom edge of border
	for (dash = 0; dash < FWIDTH + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
}
