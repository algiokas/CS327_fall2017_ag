#ifndef CHARACTER_H

#define CHARACTER_H

#include "dungeon.h"

struct Character {
    int x_pos;
    int y_pos;
    int speed;
    char symbol;
    int living;
}


