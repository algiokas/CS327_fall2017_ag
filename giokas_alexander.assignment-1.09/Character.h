#ifndef CHARACTER_H

#define CHARACTER_H

#include "Dice.h"
#include "Floor.h"

class Character
{
public:
	Character() {}
	Character(char symbol, int x, int y, int speed, int sqnum, int hitpoints, Dice damage) :
		sym(symbol), spd(speed), seq_num(sqnum), hp(hitpoints), damage(damage)
	{
		this->position.x = x;
		this->position.y = y;
	}
	virtual ~Character();

	inline char get_symbol() { return this->sym; }
	inline int x_pos() { return this->position.x; }
	inline int y_pos() { return this->position.y; }
	inline int speed() { return this->spd; }
	inline int get_hp() { return this->hp; }
	inline int sequence_num() { return this->seq_num; }

	inline bool is_alive() { return (this->hp > 0); }

    void set_location(int x, int y);
	virtual int roll_damage();
	virtual void take_damage(int amount);


protected:
	Character(char symbol, int speed, int hp, Dice damage) :
		sym(symbol), spd(speed), hp(hp), damage(damage) {}
	char sym;
	duo position;
	int spd;
	int seq_num;
	int hp;
	Dice damage;
};

#endif
