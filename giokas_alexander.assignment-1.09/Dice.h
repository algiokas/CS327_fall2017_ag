#ifndef DICE_H

#define DICE_H

#include <string>

class Dice
{
public:
	Dice() :
		base(0), quantity(0), sides(0) {}

	Dice(int base, int quantity, int sides) :
		base(base), quantity(quantity), sides(sides) {}

	inline int get_base() { return this->base; }
	inline int get_quantity() { return this->quantity; }
	inline int get_sides() { return this->sides; }

	inline void set_base(int base) { this->base = base; }
	inline void set_quantity(int quantity) { this->quantity = quantity; }
	inline void set_sides(int sides) { this->sides = sides; }

	int roll();
	std::string d_string();
	static bool parse_dice_string(std::string str, Dice *d);
	
private:
	int base;
	int quantity;
	int sides;
};

#endif
