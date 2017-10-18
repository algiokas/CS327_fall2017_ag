#ifndef PATH_H

#define PATH_H

/* 	HMODVAL is a constant that is added to the hardness value of rooms
	and corridors during corridor generation which essentially adds an
	artificial bias of the alorithm against generating corridor cells in
	rooms. Without this value (or with it set to 0), dijkstra_corridor() 
	will take a path that spends as little time carving out rock as possible. 
	However the higher the HMODVAL, the more attractive rock is compared to
	rooms and corridors. This tends to cause the generated corridors to
	meander more and creates more interesting dungeons. */
#define HMODVAL 100 

struct Floor;
struct Duo;
typedef enum isTunneling tunnel_trait;

int dijkstra_map(struct Floor *floor, tunnel_trait t);

int dijkstra_corridor(struct Floor *floor, struct Duo source, struct Duo target, int *path);

#endif