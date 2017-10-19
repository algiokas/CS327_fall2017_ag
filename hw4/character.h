#ifndef CHARACTER_H

#define CHARACTER_H

#include "dungeon.h"

struct Floor;
struct NPC;
struct PC;

struct Character {
    int loc;
    int speed;
    char symbol;
    int alive;
	int seq_num; //sequence number
	struct NPC *npc;
	struct PC *pc;
}

Character *get_character(struct Floor *floor, int x_loc, int y_loc c)
{
	int found = 0;
	int sloc = INDEX2D(x_loc, y_loc);
	int i;
	
	for (i = 0; i < floor->num_monsters; i++) {
		if (floor->characters[i].loc == sloc) {
			c = floor->characters[i];
			found = 1;
		
}


