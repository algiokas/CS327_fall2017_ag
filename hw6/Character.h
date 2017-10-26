#ifndef CHARACTER_H

#define CHARACTER_H

#include "Floor.h"

class Character
{
private:
	char symbol;
	int x_pos;
	int y_pos;
	int speed;
	bool alive;
	int sequence_num;

public:
	Character() {}
	Character(char sym, int x, int y, int spd, int sqnum);
	virtual ~Character();

	
	char symbol();
	int x();
	int y();
	int speed();
	bool is_alive();
	int sequence_num();

	bool can_see(Floor *f, Character c);
	bool attempt_move();
	void kill();
};

#endif
