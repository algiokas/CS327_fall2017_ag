#ifndef CHARACTER_H

#define CHARACTER_H

#include "dungeon.h"

struct Floor;
struct NPC;
struct PC;

struct Character {
    struct Duo loc;
    int speed;
    char symbol;
    int alive;
	int seq_num; //sequence number
	struct NPC *npc;
	struct PC *pc;
}




