#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include "Definitions.h"
#include "Floor.h"
#include "Dice.h"

std::vector<std::string> abilities = {
	"SMART", "TELE", "TUNNEL", "ERRATIC", "PASS", "PICKUP", "DESTROY","UNIQ"
};

std::vector<std::string> colors = {
	"RED", "GREEN", "BLUE", "CYAN", "YELLOW", "MAGENTA", "WHITE", "BLACK"
};

//Based on a stackoverflow answer
std::vector<std::string> split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string part;
	std::vector<std::string> tokens;
	while (getline(ss, part, delim)) {
		tokens.push_back(part);
	}
	return tokens;
}

std::vector<Monster_definition> parse_all_monsters()
{
	std::string fpath = std::string(std::getenv("HOME"));
	if (fpath.empty()) {
		fpath = ".";
	}
	fpath.append(RPATH);
	fpath.append(MONSTERS_FNAME);

	std::ifstream f(fpath);

	bool is_header = true;
	std::string current_line;
	std::vector<Monster_definition> defs;
	Monster_definition temp;
	std::vector<std::string> current_tokens;
	bool is_entry = false;
	bool is_description = false;
	std::string description = "";

	//bitset used to ensure that all parameters have been set
	std::bitset<8> parameters;

	while (std::getline(f, current_line)) {
		if (current_line.length() == 0) {
			continue;
		}

		if (is_header) {
			if (current_line.compare(MONSTER_FILE_TOKEN) != 0) {
				std::cout << current_line << std::endl;
				std::cout << "Invalid file metadata, Terminating" << std::endl;
				return defs;
			}
			is_header = false;
			continue;
		}
		//skip lines until we encounter an entry, this can also be
		//used to ignore the rest of an entry if an error is encountered
		if (!is_entry) {
			if (0 == current_line.compare(MONSTER_ENTRY_TOKEN)) {
				is_entry = true;
			}
			continue;
		}

		//Since we don't need to break up description lines into tokens,
		//check this first
		if (is_description) {
			//Check if 
			if (0 == current_line.compare(".")) {
				if (description.length() < 1) {
					//Empty description
					is_description = false;
					is_entry = false;
					parameters.reset();
					continue;
				}
				temp.set_desc(description);
				description = "";
				parameters[DESC] = 1;
				is_description = false;
				continue;	
			}
			description.append(current_line);
			description.append("\n");
			continue;
		}

		current_tokens = split(current_line, ' ');
		if (current_tokens.at(0).compare("NAME") == 0) {
			if (current_tokens.size() < 2 || parameters.test(NAME)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			std::string temp_name = "";
			for (unsigned int i = 0; i < current_tokens.size(); i++) {
				temp_name.append(current_tokens.at(i));
				if (i < current_tokens.size() - 1) {
					temp_name.append(" ");
				}
			}
			//Since my split function is can conceivably return a vector
			//with empty strings, this has to be checked seperately
			if (temp_name.length() == 0) {
				//empty name
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_name(temp_name);
			parameters[NAME] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("SYMB") == 0) {
			//empty symbol, multiple symbols, or symbol is multiple characters
			//or symbol has already been set
			if (current_tokens.size() != 2 || current_tokens.at(1).length() != 1 ||
											  parameters.test(SYMB)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_symb(current_tokens.at(1)[0]);
			parameters[SYMB] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DESC") == 0) {
			if (current_tokens.size() != 1 || parameters.test(DESC)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			is_description = true;
			continue;
		}

		if (current_tokens.at(0).compare("COLOR") == 0) {
			if (current_tokens.size() < 2 || parameters.test(COLOR)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			for (unsigned int i = 1; i < current_tokens.size(); i++) {
				if (!temp.add_color(current_tokens.at(i))) {
					is_entry = false;
					parameters.reset();
					continue;
				}
				parameters[COLOR] = 1;
				
			}
			continue;
		}

		if (current_tokens.at(0).compare("SPEED") == 0) {
			if (current_tokens.size() != 2 || parameters.test(SPEED)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			Dice d = Dice();
			if (!Dice::parse_dice_string(current_tokens.at(1), &d)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_speed(d);
			parameters[SPEED] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("ABIL") == 0) {
			if (current_tokens.size() < 2 || parameters.test(ABIL)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			for (unsigned int i = 1; i < current_tokens.size(); i++) {
				if (!temp.give_ability(current_tokens.at(i))) {
					is_entry = false;
					parameters.reset();
					continue;
				}
			}
			parameters[ABIL] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("HP") == 0) {
			if (current_tokens.size() != 2 || parameters.test(HP)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			Dice d = Dice();
			if (!Dice::parse_dice_string(current_tokens.at(1), &d)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_hp(d);
			parameters[HP] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DAM") == 0) {
			if (current_tokens.size() != 2 || parameters.test(DAM)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			Dice d = Dice();
			if (!Dice::parse_dice_string(current_tokens.at(1), &d)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_damage(d);
			parameters[DAM] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("END") == 0) {
			if (current_tokens.size() == 1 && parameters.all()) {
				defs.push_back(temp);
			}
			is_entry = false;
			parameters.reset();
			continue;
		}

		//invalid token
		is_entry = false;
		parameters.reset();
	}
	return defs;
}

bool Monster_definition::add_color(std::string color)
{
	for (unsigned int i = 0; i < colors.size(); i++) {
		if (color.compare(colors.at(i)) == 0) {
			this->color.push_back(i);
			return true;
		}
	}
	return false;
}

bool Monster_definition::give_ability(std::string ability)
{
	for (unsigned int i = 0; i < abilities.size(); i++) {
		if (ability.compare(abilities.at(i)) == 0) {
			this->ability[i] = 1;
			return true;
		}
	}
	return false;
}

void Monster_definition::print_def()
{
	std::cout << this->name << std::endl;
	std::cout << this->description;
	std::cout << this->symbol << std::endl;

	for (int c : color) {
		std::cout << colors.at(c) << " ";
	}
	std::cout << std::endl;

	std::cout << this->speed.d_string() << std::endl;
	
	for (unsigned int a = 0; a < this->ability.size(); a++) {
		if (this->ability.test(a)) {
			std::cout << abilities.at(a) << " ";
		}
	}
	std::cout << std::endl;

	std::cout << this->hitpoints.d_string() << std::endl;
	std::cout << this->damage.d_string() << std::endl;
}
