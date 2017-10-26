#ifndef CHARACTER_H

#define CHARACTER_H

#include "Floor.h"

class Character
{
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

	void kill();

private:
	char sym;
	int x_pos;
	int y_pos;
	int spd;
	bool alive;
	int seq_num;
};

#endif
