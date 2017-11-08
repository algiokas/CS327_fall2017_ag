#include "Dice.h"

#include <sstream>
#include <cctype>

int Dice::roll()
{
	int roll_sum;
	for (int i = 0; i < this->quantity; i++) {
		roll_sum += 1 + (rand() % (this->sides - 1));
	}
	return this->base + roll_sum;
}

std::string Dice::d_string()
{
	std::ostringstream output;
	output << this->base << "+" << this->quantity << "d" << this->sides;
	return output.str();
}

bool Dice::parse_dice_string(std::string str, Dice *d)
{
	std::stringstream ss;
	for (unsigned int i = 0; i < str.length(); i++) {
		if (isdigit(str.at(i))) {
			ss << str.at(i);
			if (i == str.length() - 1) {
				int sides;
				ss >> sides;
				d->set_sides(sides);
			}
		} 
		else if (str.at(i) == 'd') {
			int base;
			ss >> base;
			d->set_base(base);
			ss.str("");
			ss.clear();
		} 
		else if (str.at(i) == '+') {
			int q;
			ss >> q;
			d->set_quantity(q);
			ss.str("");
			ss.clear();
		}
		else {
			return false;
		}
	}
	return true;
}
