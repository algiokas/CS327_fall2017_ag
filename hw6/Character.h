#pragma once

#include "Floor.h"

class Character
{
private:
	char symbol;
	int position;
	int speed;
	bool alive;
	int sequence_num;

public:
	Character(char sym, int pos, int spd, int sqnum);
	virtual ~Character();

	
	char symbol();
	int position();
	int speed();
	bool is_alive();
	int sequence_num();

	bool can_see(Floor *f, Character c);
	bool attempt_move();
	void kill();
};
