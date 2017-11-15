#include <array>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <cmath>
#include <arpa/inet.h>
#include <algorithm>

#include "PQueue.h"
#include "Floor.h"
#include "Character.h"
#include "PQueue.h"
#include "PC.h"
#include "Object.h"

//array indices correspond to direction enum values, so if n = get_neighbors(x, y);
//then n[north] is the index of the dungeon cell in the northern direction


Floor::Floor()
{
	std::cout << "Constructing a Floor" << std::endl;
	width = FWIDTH;
	height = FHEIGHT;
	num_rooms = 0;
	pc_loc = 0;
	num_monsters = 0;
	max_monsters = 0;
	time = 0;

	empty_floor();
	gen_rooms();
	add_corridors();
	add_stairs();
}

Floor::Floor(std::string filename)
{
	std::cout << "Constructing a Floor from file" << std::endl;

	width = FWIDTH;
	height = FHEIGHT;
	num_rooms = 0;
	pc_loc = 0;
	num_monsters = 0;
	max_monsters = 0;
	time = 0;

	load_from_file(filename);
	add_stairs();
}

Floor::~Floor()
{
	for (Character *c : char_map) {
		if (c && (c->x_pos() != linearX(pc_loc) || c->y_pos() != linearY(pc_loc))) {
			delete c;
		}
	}
	for (Object *o : obj_map) {
		if (o) {
			delete o;
		}
	}
}

void Floor::set_cell_type(int x, int y, CType input_type)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
	}
	type_map[index2d(x, y)] = input_type;
}

void Floor::set_cell_hardn(int x, int y, int input_hardn)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
	}
	if (input_hardn < 0 || input_hardn > 255) {
		std::cout << "Invalid hardness value";
	}
	hard_map[index2d(x, y)] = input_hardn;
}

void Floor::set_cell_dist(int x, int y, int dist)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
	}
	dist_map[index2d(x, y)] = dist;
}

void Floor::set_cell_dist_tunnel(int x, int y, int dist)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
	}
	dist_map_tunnel[index2d(x, y)] = dist;
}

bool Floor::place_character(int x, int y, Character * c)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds" << std::endl;
		return false;
	}
	if (char_map[index2d(x, y)]) {
		std::cout << "Cell occupied" << std::endl;
		return false;
	}
	char_map[index2d(x, y)] = c;
	return true;
}

bool Floor::place_object(int x, int y, Object * o)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds" << std::endl;
		return false;
	}
	if (obj_map[index2d(x, y)]) {
		std::cout << "Cell occupied" << std::endl;
		return false;
	}
	obj_map[index2d(x, y)] = o;
	return true;
}

std::array<int, 8> Floor::get_neighbors(int x, int y)
{
	int index = index2d(x, y);
	int top = index - FWIDTH;
	int right = index + 1;
	int bottom = index + FWIDTH;
	int left = index - 1;

	int topright = top + 1;
	int botright = bottom + 1;
	int botleft = bottom - 1;
	int topleft = top - 1;

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

std::vector<int> Floor::bresenham_line(int start_x, int start_y, int end_x, int end_y)
{
	std::vector<int> line;

    const bool steep = (std::abs(end_y - start_y) > std::abs(end_x - start_x));
    if (steep) {
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
    }

    if (start_x > end_x) {
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }

    const double dx = end_x - start_x;
    const double dy = std::abs(end_y - start_y);

    double error = (dx / 2.0);
    const int y_step = (start_y < end_y) ? 1 : -1;
    int y = start_y;

    const int x_dest = end_x;

    for (int x = start_x; x < x_dest; x++) {
        if (steep) {
            line.push_back(index2d(y, x));
        } else {
            line.push_back(index2d(x, y));
        }
        
        error -= dy;
        if (error < 0) {
            y += y_step;
            error += dx;
        }
    }
    return line;
}

CType Floor::get_type(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
	}
	return type_map[index2d(x, y)];
}

int Floor::get_hardn(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		std::cout << "Cell index out of bounds";
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
	return char_map[index2d(x, y)];
}

Object * Floor::get_object(int x, int y)
{
	return obj_map[index2d(x, y)];
}

duo Floor::get_pc_location()
{
	duo l = duo();
	l.x = linearX(pc_loc);
	l.y = linearY(pc_loc);

	return l;
}

int Floor::get_width()
{
	return this->width;
}

int Floor::get_height()
{
	return this->height;
}

void Floor::empty_floor()
{
	std::cout << "Generating empty floor..." << std::endl;
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				set_cell_type(x, y, immutable_c);
				set_cell_hardn(x, y, 255);
			}
			else {
				int rand_hardness = 1 + (rand() % 254);
				set_cell_type(x, y, rock_c);
				set_cell_hardn(x, y, rand_hardness);
			}
		}
	}
}

void Floor::place_room(room * r)
{
	std::cout << "placing room..." << std::endl;
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
	std::cout << "checking intersection with " << num_rooms << " rooms..." << std::endl;
	if (num_rooms == 0) {
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
		
		bool x_intersect = false;
		bool y_intersect = false;
		
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
	std::cout << "generating rooms..." << std::endl;
	double floorsize = width * height;
	double freespace = floorsize; //the total amount of free space within which to place rooms
	int roomIter = 0;
	room newRoom = room(); //blank room that we can fill with data and pass to functions

	while ((num_rooms < MINROOMS || (freespace / floorsize) > 1 - ROOMDENSITY)
		&& num_rooms < MAXROOMS) {
		//Rooms should never touch the edge
		newRoom.loc.x = 1 + (rand() % (width - MINROOMWIDTH - 1));
		newRoom.loc.y = 1 + (rand() % (height - MINROOMHEIGHT - 1));

		int maxwidth = std::min(width - newRoom.loc.x - 1, MAXROOMWIDTH);
		int maxheight = std::min(height - newRoom.loc.y - 1, MAXROOMHEIGHT);
		if (maxwidth > MINROOMWIDTH) {
			newRoom.dims.x = MINROOMWIDTH + (rand() % (maxwidth - MINROOMWIDTH));
		}
		else {
			newRoom.dims.x = MINROOMWIDTH;
		}

		if (maxheight > MINROOMWIDTH) {
			newRoom.dims.y = MINROOMHEIGHT + (rand() % (maxheight - MINROOMHEIGHT));
		}
		else {
			newRoom.dims.y = MINROOMHEIGHT;
		}

		if (!check_intersection(&newRoom)) {
			std::cout << "No Intersection" << std::endl;
			rooms.push_back(newRoom);
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
	std::cout << "Generating random room location..." << std::endl;
	int r = rand() % num_rooms;

	struct duo out;
	out.x = rooms[r].loc.x + (rand() % rooms[r].dims.x);
	out.y = rooms[r].loc.y + (rand() % rooms[r].dims.y);

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
	std::cout << "calculating new corridor..." << std::endl;
	std::array<int, FWIDTH * FHEIGHT> dist;
	std::array<int, FWIDTH * FHEIGHT> prev;
	std::array<bool, FWIDTH * FHEIGHT> visited;
	std::vector<int> path;
	
	PQueue pq = PQueue();

	int current = -1;
	int in;
	int out;
	int iter;

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
	pq.insert(0, in);

	//primary search loop 
	while (!pq.is_empty()) { // queue is not empty
		out = pq.remove_min();
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
					pq.insert(dist[left], in);
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
					pq.insert(dist[top], in);
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
					pq.insert(dist[right], in);
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
					pq.insert(dist[bottom], in);
					visited[bottom] = 1;
				}
			}
		}
	}
	if (current == -1) {
		std::cout << "dijkstra_corridor() error : queue failure";
	}
	
	int pathIter = 1;
	path.push_back(current);
	while (prev[current] != source && pathIter < 100) {
		path.push_back(prev[current]);
		current = prev[current];
		pathIter++;
	}

	return path;
}

void Floor::draw_path(std::vector<int> path)
{
	std::cout << "drawing path from " << path.at(0) << " to " << path.at(path.size() -1) << std::endl;
	if (path.empty()) {
		std::cout << "invalid draw path";
	}
	uint32_t i; //prevents comparison to path.size() from giving a warning
	//while(path[i]) {
	for (i = 0; i < path.size(); i++) {
		int pathx = (linearX(path[i]));
		int pathy = (linearY(path[i]));
		std::cout << get_type(pathx, pathy);
		if (get_type(pathx, pathy) == rock_c) {
			set_cell_type(pathx, pathy, corridor_c);
			set_cell_hardn(pathx, pathy, 0);
		}
	}
	std::cout << std::endl;
}

void Floor::add_corridors()
{
	std::cout << "adding corridors..." << std::endl;
	int r, next;

	for (r = 0; r < num_rooms; r++) {
		if (r == num_rooms - 1) {
			next = 0;
		} else {
			next = r + 1;
		}
		int ax = rooms[r].loc.x + (rooms[r].dims.x / 2);
		int ay = rooms[r].loc.y + (rooms[r].dims.y / 2);
		int bx = rooms[next].loc.x + (rooms[next].dims.x / 2);
		int by = rooms[next].loc.y + (rooms[next].dims.y / 2);

		int roomACentroid = index2d(ax, ay);
		int roomBCentroid = index2d(bx, by);

		std::vector<int> path = dijkstra_corridor(roomACentroid, roomBCentroid);
		draw_path(path);
	}
}

void Floor::add_stairs()
{
	duo up = rand_room_location();
	duo down = rand_room_location();

	type_map[index2d(up.x, up.y)] = stairs_up_c;
	hard_map[index2d(up.x, up.y)] = 0;
	type_map[index2d(down.x, down.y)] = stairs_down_c;
	hard_map[index2d(down.x, down.y)] = 0;
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
				std::cout << "input file has invalid version";
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
				std::cout << "input file has invalid size";
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

void Floor::spawn_pc(PC* player)
{
	std::cout << "Spawning player character at (" << player->x_pos() << ", " << player->y_pos() << ")..." << std::endl;
	this->pc = player;
	this->pc_loc = index2d(player->x_pos(), player->y_pos());
	this->char_map[pc_loc] = player;
}

void Floor::place_at_stairs(bool up)
{
	duo stairs_loc;
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			if (up && get_type(x, y) == stairs_up_c) {
				stairs_loc.x = x;
				stairs_loc.y = y;
			}
			else if (!up && get_type(x, y) == stairs_down_c) {
				stairs_loc.x = x;
				stairs_loc.y = y;
			}
		}
	}
	this->pc->set_location(stairs_loc.x, stairs_loc.y);

	int stairs_idx = index2d(stairs_loc.x, stairs_loc.y);
	this->char_map[stairs_idx] = char_map[pc_loc];
	this->char_map[pc_loc] = NULL;
	this->pc_loc = stairs_idx;
	this->pc->update_vision();
}

bool Floor::move_pc(direction d)
{
	std::array<int, 8> n = get_neighbors(linearX(pc_loc), linearY(pc_loc));
	CType next = type_map.at(n[d]);
	if (next != rock_c && next != immutable_c) {
		std::cout << "moved" << std::endl;
		char_map[n[d]] = pc;
		char_map[pc_loc] = NULL;
		pc_loc = n[d];
		pc->set_location(linearX(n[d]), linearY(n[d]));
		pc->update_vision();
		update_dist(tunneling);
		update_dist(non_tunneling);
		return true;
	}
	return false;
}

bool Floor::pc_can_see(int x, int y)
{
	return pc->can_see(x, y);
}

bool Floor::pc_has_seen(int x, int y)
{
	return pc->has_seen(x, y);
}

void Floor::save_to_file()
{

	save_to_file(DEFAULT_FNAME);
}

void Floor::save_to_file(std::string filename)
{
	std::cout << "saving to file..." << std::endl;
	std::string fpath;
	std::ofstream f;
	std::string marker = FMARKER;
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

	uint32_t version_be = htonl(version);
	uint32_t fsize_be = htonl(fsize);

	//begin file write
	f.write(marker.c_str(), marker.length());
	f.write(reinterpret_cast<const char *>(&version_be), sizeof version);
	f.write(reinterpret_cast<const char *>(&fsize_be), sizeof fsize);

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
	std::cout << "Updating distance map..." << std::endl;
	std::array<int, FWIDTH * FHEIGHT> dist;
	std::array<int, FWIDTH * FHEIGHT> prev;
	std::array<bool, FWIDTH * FHEIGHT> visited;
	PQueue pq = PQueue();
	int current = -1;
	int in;
	int out;
	int iter;
	int sourceIndex = pc_loc;

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
	pq.insert(0, in);

	//primary search loop 
	while (!pq.is_empty()) { // queue is not empty
		out = pq.remove_min();
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
						pq.insert(dist[n[i]], in);
						visited[n[i]] = 1;
					}
				}
			}
		}
	}
	if (current == -1) {
		std::cout << "dijkstra_map() error : queue failure";
	}

	bool success;
	int mapiter;
	if (t == non_tunneling) {
		for (mapiter = 0; mapiter < width * height; mapiter++) {
			dist_map[mapiter] = dist[mapiter];
		}
		success = true;
	}
	else if (t == tunneling) {
		for (mapiter = 0; mapiter < width * height; mapiter++) {
			dist_map_tunnel[mapiter] = dist[mapiter];
		}
		success = true;
	}
	else {
		std::cout << "dijkstra_map() error : invalid tunneling value";
	}

	if (!success) {
		std::cout << "dijkstra_map() error : map modification error";
	}
}

void Floor::print_floor()
{
	int row, col, dash;
	std::cout << std::endl << "Dungeon Map" << std::endl;
	//top edge of border
	for (dash = 0; dash < this->width + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
	for (col = 0; col < height; col++) {
		std::cout << "|";
		for (row = 0; row < width; row++) {
			if (char_map.at(index2d(row, col)) != NULL) {
				std::cout << char_map.at(index2d(row, col))->get_symbol();
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
				case(stairs_up_c):
					std::cout << "<";
					break;
				case(stairs_down_c):
					std::cout << ">";
					break;
				default:
					std::cout << "X"; //indicates an error
				}
			}
		}
		std::cout << "|" << std::endl;
	}
	//bottom edge of border
	for (dash = 0; dash < this->width + 2; dash++) {
		std::cout << "-";
	}
	std::cout << std::endl;
}
