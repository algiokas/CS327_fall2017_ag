#include <array>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <byteswap.h>

#include "pqueue.h"
#include "Floor.h"
#include "Character.h"
#include "PC.h"

enum direction {
	north,
	northeast,
	east,
	southeast,
	south,
	southwest,
	west,
	northwest
};

//array indices correspond to enum values, so if n = get_neighbors(x, y);
//then n[north] is the index of the dungeon cell in the northern direction
std::array<int, 8> get_neighbors(int x, int y)
{
	int index = index2d(x, y);
	int left = index - 1;
	int top = index - FWIDTH;
	int right = index + 1;
	int bottom = index + FHEIGHT;
	int topleft = top - 1;
	int topright = top + 1;
	int botright = bottom + 1;
	int botleft = bottom - 1;

	std::array<int, 8> n;

	n[0] = top;
	n[1] = topright;
	n[2] = right;
	n[3] = botright;
	n[4] = bottom;
	n[5] = botleft;
	n[6] = left;
	n[7] = topleft;

	return n;
}

Floor::Floor(PC* pc)
{
	width = FWIDTH;
	height = FHEIGHT;
	num_rooms = 0;
	pc_loc = 0;
	num_monsters = 0;
	max_monsters = 0;
	time = 0;

	this->pc = pc;

	rooms = std::vector<struct room>();
	type_map = std::array<CType, FWIDTH * FHEIGHT>();
	hard_map = std::array<int, FWIDTH * FHEIGHT>();
	dist_map = std::array<int, FWIDTH * FHEIGHT>();
	dist_map_tunnel = std::array<int, FWIDTH * FHEIGHT>();
	char_map = std::array<Character *, FWIDTH * FHEIGHT>();

	empty_floor();
	gen_rooms();
	add_corridors();
	duo spawn = rand_room_location();
	place_pc(spawn.x, spawn.y);
}

Floor::Floor(PC* pc, std::string filename)
{
	width = FWIDTH;
	height = FHEIGHT;
	num_rooms = 0;
	pc_loc = index2d(pc->x, pc->y);
	num_monsters = 0;
	max_monsters = 0;
	time = 0;

	this->pc = pc;

	rooms = std::vector<struct room>();
	type_map = std::array<CType, FWIDTH * FHEIGHT>();
	hard_map = std::array<int, FWIDTH * FHEIGHT>();
	dist_map = std::array<int, FWIDTH * FHEIGHT>();
	dist_map_tunnel = std::array<int, FWIDTH * FHEIGHT>();
	char_map = std::array<Character *, FWIDTH * FHEIGHT>();

	load_from_file(filename);
	duo spawn = rand_room_location();
	place_pc(spawn.x, spawn.y);
}

Floor::~Floor()
{
}

void Floor::set_cell_type(int x, int y, CType input_type)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	type_map[index2d(x, y)] = input_type;
}

void Floor::set_cell_hardn(int x, int y, int input_hardn)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	if (input_hardn < 0 || input_hardn > 255) {
		throw "Invalid hardness value";
	}
	hard_map[index2d(x, y)] = input_hardn;
}

void Floor::set_cell_dist(int x, int y, int dist)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	dist_map[index2d(x, y)] = dist;
}

void Floor::set_cell_dist_tunnel(int x, int y, int dist)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	dist_map_tunnel[index2d(x, y)] = dist;
}

void Floor::place_character(int x, int y, Character * c)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	char_map[index2d(x, y)] = c;
}

CType Floor::get_type(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	return type_map[index2d(x, y)];
}

int Floor::get_hardn(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw "Cell index out of bounds";
	}
	return hard_map[index2d(x, y)];
}

int Floor::get_dist(int x, int y)
{
	return dist_map[index2d(x, y)];
}

int Floor::get_dist_tunnel(int x, int y)
{
	return dist_map_tunnel[index2d(x, y)];
}

Character * Floor::get_character(int x, int y)
{
	return char_map[index2d(x, y)];;
}

void Floor::empty_floor()
{
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
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
		newRoom.loc.x = 1 + (std::rand() % (width - MINROOMWIDTH - 1));
		newRoom.loc.y = 1 + (std::rand() % (height - MINROOMHEIGHT - 1));

		int maxwidth = width - newRoom.loc.x - 1;
		int maxheight = height - newRoom.loc.y - 1;
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

inline int pf_weight(int hardness) {
	if (hardness == 255) {
		return INT_MAX;
	}
	return 1 + hardness / 85;
}

std::vector<int> Floor::dijkstra_corridor(int source, int target)
{
	std::array<int, FWIDTH * FHEIGHT> dist;
	std::array<int, FWIDTH * FHEIGHT> prev;
	std::array<bool, FWIDTH * FHEIGHT> visited;
	std::vector<int> path;
	
	struct PQueue pq;

	int current = -1;
	int in;
	int out;
	int iter;

	init_PQ(&pq, (width * height / 2), sizeof(int));

	//dist: set values to "Infinity" (INT_MAX) for all nodes except source, which is set to 0
	//visited: set values to 0 for all nodes except source, which is set to 1
	//prev: set all values to -1
	for (iter = 0; iter < width * height; iter++) {
		if (iter == source) {
			dist[iter] = 0;
			visited[iter] = 1;
		}
		else {
			dist[iter] = INT_MAX;
			visited[iter] = 0;
		}
		prev[iter] = -1;
	}
	in = source;
	insert(&pq, 0, &in);

	//primary search loop 
	while (pq.size) { // queue is not empty
		remove_min(&pq, &out);
		current = out;
		if (current == target) {
			break;
		}
		//Left neighbor
		if ((current % width) > 1) {
			int left = current - 1;
			int hmod = HMODVAL * (type_map[left] == room_c);
			if (dist[left] > dist[current] + hard_map[left] + hmod) {
				dist[left] = dist[current] + hard_map[left] + hmod;
				prev[left] = current;
				if (!visited[left]) {
					in = left;
					insert(&pq, dist[left], &in);
					visited[left] = 1;
				}
			}
		}
		//top neighbor
		if ((current / width) > 1) {
			int top = current - width;
			int hmod = HMODVAL * (type_map[top] == room_c);
			if (dist[top] > dist[current] + hard_map[top] + hmod) {
				dist[top] = dist[current] + hard_map[top] + hmod;
				prev[top] = current;
				if (!visited[top]) {
					in = top;
					insert(&pq, dist[top], &in);
					visited[top] = 1;
				}
			}
		}
		//right neighbor
		if ((current % width) < width - 1) {
			int right = current + 1;
			int hmod = HMODVAL * (type_map[right] == room_c);
			if (dist[right] > dist[current] + hard_map[right] + hmod) {
				dist[right] = dist[current] + hard_map[right] + hmod;
				prev[right] = current;
				if (!visited[right]) {
					in = right;
					insert(&pq, dist[right], &in);
					visited[right] = 1;
				}
			}
		}
		//bottom neighbor
		if ((current / width) < height - 1) {
			int bottom = current + width;
			int hmod = HMODVAL * (type_map[bottom] == room_c);
			if (dist[bottom] > dist[current] + hard_map[bottom] + hmod) {
				dist[bottom] = dist[current] + hard_map[bottom] + hmod;
				prev[bottom] = current;
				if (!visited[bottom]) {
					in = bottom;
					insert(&pq, dist[bottom], &in);
					visited[bottom] = 1;
				}
			}
		}
	}
	if (current == -1) {
		throw "dijkstra_corridor() error : queue failure";
	}
	
	int pathIter = 1;
	path.push_back(current);
	while (prev[current] != source && pathIter < 100) {
		path.push_back(prev[current]);
		current = prev[current];
		pathIter++;
	}

	delete_PQ(&pq);

	return path;
}

void Floor::draw_path(std::vector<int> path)
{
	if (path.empty()) {
		throw "invalid draw path";
	}
	int i;
	//while(path[i]) {
	for (i = 0; i < path.size(); i++) {
		int pathx = (linearX(path[i]));
		int pathy = (linearY(path[i]));
		if (get_type(pathx, pathy) == rock_c) {
			set_cell_type(pathx, pathy, corridor_c);
			set_cell_hardn(pathx, pathy, 0);
		}
	}
}

void Floor::add_corridors()
{
	int r, next;

	for (r = 0; r < num_rooms; r++) {
		int roomACentroid, roomBCentroid;
		if (r == num_rooms - 1) {
			next = 0;
		} else {
			next = r + 1;
		}
		int ax = rooms[r].loc.x + (rooms[r].dims.x / 2);
		int ay = rooms[r].loc.y + (rooms[r].dims.y / 2);
		int bx = rooms[next].loc.x + (rooms[next].dims.x / 2);
		int by = rooms[next].loc.y + (rooms[next].dims.y / 2);

		roomACentroid = index2d(ax, ay);
		roomBCentroid = index2d(bx, by);

		std::vector<int> path = dijkstra_corridor(roomACentroid, roomBCentroid);
		draw_path(path);
	}
}

void Floor::load_from_file(std::string filename)
{

	std::string fpath;
	std::ifstream f;
	std::string marker = FMARKER;
	int i, ch;
	int32_t version = PVERSION;

	width = width;
	height = height;
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

void Floor::place_pc(int x, int y)
{
	pc_loc = index2d(x, y);
	char_map[pc_loc] = pc;

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
	fsize = headersize + (width * height) + (4 * num_rooms);

	f.open(fpath, std::ios_base::out | std::ios_base::binary);

	//begin file write
	f.write(marker.c_str(), marker.length());
	f.write(bswap_32(version), sizeof version);
	f.write(bswap_32(fsize), sizeof fsize);

	int i;
	for (i = 0; i < width * height; i++) {
		int x = (i % width);
		int y = (i / width);
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

void Floor::update_dist(isTunneling t)
{
	std::array<int, FWIDTH * FHEIGHT> dist;
	std::array<int, FWIDTH * FHEIGHT> prev;
	std::array<bool, FWIDTH * FHEIGHT> visited;
	struct PQueue pq;
	int current = -1;
	int in;
	int out;
	int iter;
	int sourceIndex = pc_loc;

	init_PQ(&pq, (width * height / 2), sizeof(int));

	//dist: set values to "Infinity" (INT_MAX) for all nodes except source, which is set to 0
	//visited: set values to 0 for all nodes except source, which is set to 1
	//prev: set all values to -1
	for (iter = 0; iter < width * height; iter++) {
		if (iter == sourceIndex) {
			dist[iter] = 0;
			visited[iter] = true;
		}
		else {
			dist[iter] = INT_MAX;
			visited[iter] = false;
		}
		prev[iter] = -1;
	}
	in = sourceIndex;
	insert(&pq, 0, &in);

	//primary search loop 
	while (pq.size) { // queue is not empty
		remove_min(&pq, &out);
		current = out;
		/*
		int left = current - 1;
		int top = current - width;
		int right = current + 1;
		int bottom = current + width;
		int topleft = top - 1;
		int topright = top + 1;
		int botright = bottom + 1;
		int botleft = bottom - 1;
		*/

		//array of neighbors for iteration
		std::array<int, 8> n = get_neighbors(linearX(current), linearY(current));
		int eweight = pf_weight(hard_map[current]);

		int i;
		for (i = 0; i < 8; i++) {
			if (eweight != INT_MAX && (hard_map[n[i]] == 0 || t == tunneling)) {
				if (dist[n[i]] > dist[current] + eweight) {
					dist[n[i]] = dist[current] + eweight;
					prev[n[i]] = current;
					if (!visited[n[i]]) {
						in = n[i];
						insert(&pq, dist[n[i]], &in);
						visited[n[i]] = 1;
					}
				}
			}
		}
	}
	if (current == -1) {
		throw "dijkstra_map() error : queue failure\n";
	}

	int mapiter;
	int success;
	if (t == non_tunneling) {
		for (mapiter = 0; mapiter < width * height; mapiter++) {
			dist_map[mapiter] = dist[mapiter];
		}
		success = 0;
	}
	else if (t == tunneling) {
		for (mapiter = 0; mapiter < width * height; mapiter++) {
			dist_map_tunnel[mapiter] = dist[mapiter];
		}
		success = 0;
	}
	else {
		success = 1;
	}
	delete_PQ(&pq);
}

void Floor::print_floor()
{
	int row, col, dash;
	std::cout << std::endl << "Dungeon Map" << std::endl;
	//top edge of border
	for (dash = 0; dash < width + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
	for (col = 0; col < height; col++) {
		std::cout << "|";
		for (row = 0; row < width; row++) {
			if (index2d(row, col) == pc_loc) {
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
	for (dash = 0; dash < width + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
}
