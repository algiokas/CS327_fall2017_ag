#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include "Definitions.h"
#include "Floor.h"
#include "Dice.h"
#include "NPC.h"
#include "Object.h"

std::vector<std::string> abilities = {
	"SMART", "TELE", "TUNNEL", "ERRATIC", "PASS", "PICKUP", "DESTROY","UNIQ"
};

std::vector<std::string> colors = {
	"RED", "GREEN", "BLUE", "CYAN", "YELLOW", "MAGENTA", "WHITE", "BLACK"
};

std::vector<std::string> item_types = {
	"NO_TYPE", "WEAPON", "OFFHAND", "RANGED", "ARMOR", "HELMET", "CLOAK", "GLOVES", "BOOTS", "RING", "AMULET", "LIGHT", //EQUIPMENT IN THIS ROW
	"SCROLL", "BOOK", "FLASK", "GOLD", "AMMUNITION", "FOOD", "WAND", "CONTAINER" //NON-EQUIPMENT IN THIS ROW
};

extern const char object_symbol[] = {
	'*', /* objtype_no_type */
	'|', /* objtype_WEAPON */
	')', /* objtype_OFFHAND */
	'}', /* objtype_RANGED */
	'~', /* objtype_LIGHT */
	'[', /* objtype_ARMOR */
	']', /* objtype_HELMET */
	'(', /* objtype_CLOAK */
	'{', /* objtype_GLOVES */
	'\\', /* objtype_BOOTS */
	'"', /* objtype_AMULET */
	'=', /* objtype_RING */
	'`', /* objtype_SCROLL */
	'?', /* objtype_BOOK */
	'!', /* objtype_FLASK */
	'$', /* objtype_GOLD */
	'/', /* objtype_AMMUNITION */
	',', /* objtype_FOOD */
	'-', /* objtype_WAND */
	'%', /* objtype_CONTAINER */
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
				parameters[DESC_M] = 1;
				is_description = false;
				continue;	
			}
			description.append(current_line);
			description.append("\n");
			continue;
		}

		current_tokens = split(current_line, ' ');
		if (current_tokens.at(0).compare("NAME") == 0) {
			if (current_tokens.size() < 2 || parameters.test(NAME_M)) {
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
			parameters[NAME_M] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("SYMB") == 0) {
			//empty symbol, multiple symbols, or symbol is multiple characters
			//or symbol has already been set
			if (current_tokens.size() != 2 || current_tokens.at(1).length() != 1 ||
											  parameters.test(SYMB_M)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			temp.set_symb(current_tokens.at(1)[0]);
			parameters[SYMB_M] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DESC") == 0) {
			if (current_tokens.size() != 1 || parameters.test(DESC_M)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			is_description = true;
			continue;
		}

		if (current_tokens.at(0).compare("COLOR") == 0) {
			if (current_tokens.size() < 2 || parameters.test(COLOR_M)) {
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
				parameters[COLOR_M] = 1;
				
			}
			continue;
		}

		if (current_tokens.at(0).compare("SPEED") == 0) {
			if (current_tokens.size() != 2 || parameters.test(SPEED_M)) {
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
			parameters[SPEED_M] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("ABIL") == 0) {
			if (current_tokens.size() < 2 || parameters.test(ABIL_M)) {
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
			parameters[ABIL_M] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("HP") == 0) {
			if (current_tokens.size() != 2 || parameters.test(HP_M)) {
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
			parameters[HP_M] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DAM") == 0) {
			if (current_tokens.size() != 2 || parameters.test(DAM_M)) {
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
			parameters[DAM_M] = 1;
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

std::vector<Object_definition> parse_all_objects()
{
	std::string fpath = std::string(std::getenv("HOME"));
	if (fpath.empty()) {
		fpath = ".";
	}
	fpath.append(RPATH);
	fpath.append(OBJECTS_FNAME);

	std::ifstream f(fpath);

	bool is_header = true;
	std::string current_line;
	std::vector<Object_definition> defs;
	Object_definition temp;
	std::vector<std::string> current_tokens;
	bool is_entry = false;
	bool is_description = false;
	std::string description = "";

	//bitset used to ensure that all parameters have been set
	std::bitset<12> parameters;

	while (std::getline(f, current_line)) {
		
		if (current_line.length() == 0) {
			continue;
		}

		if (is_header) {
			if (current_line.compare(OBJECT_FILE_TOKEN) != 0) {
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
			if (0 == current_line.compare(OBJECT_ENTRY_TOKEN)) {
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
				parameters[DESC_O] = 1;
				is_description = false;
				continue;
			}
			description.append(current_line);
			description.append("\n");
			continue;
		}

		current_tokens = split(current_line, ' ');
		if (current_tokens.at(0).compare("NAME") == 0) {
			if (current_tokens.size() < 2 || parameters.test(NAME_O)) {
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
			parameters[NAME_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DESC") == 0) {
			if (current_tokens.size() != 1 || parameters.test(DESC_O)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			is_description = true;
			continue;
		}

		if (current_tokens.at(0).compare("TYPE") == 0) {
			if (current_tokens.size() != 2 || parameters.test(TYPE_O)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			if (!temp.set_type(current_tokens.at(1))) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			parameters[TYPE_O] = 1;
			continue;
			
		}

		if (current_tokens.at(0).compare("COLOR") == 0) {
			if (current_tokens.size() != 2 || parameters.test(COLOR_O)) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			if (!temp.set_color(current_tokens.at(1))) {
				is_entry = false;
				parameters.reset();
				continue;
			}
			parameters[COLOR_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("HIT") == 0) {
			if (current_tokens.size() != 2 || parameters.test(HIT_O)) {
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
			temp.set_hit(d);
			parameters[HIT_O] = 1;
			continue;
		}

		
		if (current_tokens.at(0).compare("DAM") == 0) {
			if (current_tokens.size() != 2 || parameters.test(DAM_O)) {
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
			temp.set_dam(d);
			parameters[DAM_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DODGE") == 0) {
			if (current_tokens.size() != 2 || parameters.test(DODGE_O)) {
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
			temp.set_dodge(d);
			parameters[DODGE_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("DEF") == 0) {
			if (current_tokens.size() != 2 || parameters.test(DEF_O)) {
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
			temp.set_def(d);
			parameters[DEF_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("WEIGHT") == 0) {
			if (current_tokens.size() != 2 || parameters.test(WEIGHT_O)) {
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
			temp.set_weight(d);
			parameters[WEIGHT_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("SPEED") == 0) {
			if (current_tokens.size() != 2 || parameters.test(SPEED_O)) {
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
			parameters[SPEED_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("ATTR") == 0) {
			if (current_tokens.size() != 2 || parameters.test(ATTR_O)) {
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
			temp.set_attr(d);
			parameters[ATTR_O] = 1;
			continue;
		}

		if (current_tokens.at(0).compare("VAL") == 0) {
			if (current_tokens.size() != 2 || parameters.test(VAL_O)) {
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
			temp.set_val(d);
			parameters[VAL_O] = 1;
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

bool Monster_definition::has_ability(abilities_index a)
{
	return ability.test(a);
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

NPC * Monster_definition::generate_NPC()
{
	NPC *npc = new NPC(
		this->name, 
		this->symbol, 
		this->description, 
		this->color.at(0),
		this->speed.roll(), 
		this->ability.to_ulong(), 
		this->hitpoints.roll(), 
		this->damage
	);

	this->times_generated++;
	return npc;
}

bool Object_definition::set_type(std::string type)
{
	for (unsigned int i = 0; i < item_types.size(); i++) {
		if (type.compare(item_types.at(i)) == 0) {
			this->type = (type_index)i;
			if (i <= (int)LIGHT) {
				this->is_equipment = true;
			}
			else {
				this->is_equipment = false;
			}
			return true;
		}
	}
	return false;
}

bool Object_definition::set_color(std::string color)
{
	for (unsigned int i = 0; i < colors.size(); i++) {
		if (color.compare(colors.at(i)) == 0) {
			this->color = i;
			return true;
		}
	}
	return false;
}

void Object_definition::print_def()
{
	std::cout << this->name << std::endl;
	std::cout << this->description;
	std::cout << item_types.at(this->type) << std::endl;
	std::cout << colors.at(this->color) << std::endl;
	std::cout << this->hit.d_string() << std::endl;
	std::cout << this->dam.d_string() << std::endl;
	std::cout << this->dodge.d_string() << std::endl;
	std::cout << this->def.d_string() << std::endl;
	std::cout << this->weight.d_string() << std::endl;
	std::cout << this->speed.d_string() << std::endl;
	std::cout << this->attr.d_string() << std::endl;
	std::cout << this->val.d_string() << std::endl;
}
