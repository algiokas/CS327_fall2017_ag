#include "PC.h"
#include "Floor.h"
#include "Object.h"
#include "IOhandler.h"
#include "Dungeon.h"

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <climits>

//intialize instance pointer
PC *PC::s_instance = 0;

PC * PC::instance()
{
	if (!s_instance) {
		s_instance = new PC();
	}
	return s_instance;
}

void PC::reset()
{
	if (s_instance) {
		delete s_instance;
	}
}

unsigned int PC::roll_damage()
{
	if (god_mode) {
		return INT_MAX - 1;
	}
	unsigned int dmg_total = this->damage.roll();

	for (unsigned int i = 0; i < equipment.size(); i++) {
		if (equipment[i]) {
			dmg_total += equipment[i]->roll_damage();
		}
	}
	return dmg_total;

}

void PC::take_damage(int amount)
{
	if (god_mode) {
		return;
	}
	int resistance = 0;

	for (unsigned int i = 0; i < equipment.size(); i++) {
		if (equipment[i]) {
			resistance += equipment[i]->defense;
		}
	}
	this->hp -= (amount - resistance);
}

unsigned int PC::get_speed()
{
	unsigned int speed = this->spd;

	for (unsigned int i = 0; i < equipment.size(); i++) {
		if (equipment[i]) {
			speed += equipment[i]->speed;
		}
	}
	if (speed < 1) { speed = 1; }

	return speed;

}

void PC::do_turn()
{
}

bool PC::pick_up_object(Object * obj)
{
	for (unsigned int i = 0; i < inventory.size(); i++) {
		if (!inventory[i]) {
			inventory[i] = obj;
			std::string pickup_message = "You picked up ";
			pickup_message += obj->name;
			IO_handler::queue_message(pickup_message);
			return true;
		}
	}
	//full inventory
	std::string full_message = "Your inventory was too full for ";
	full_message += obj->name;
	IO_handler::queue_message(full_message);
	return false;
}

bool PC::equip_object(int slot)
{
	if (!inventory.at(slot)) {
		return false;
	}
	for (int t = 1; t < (int)SCROLL; t++) {
		if ((int)inventory.at(slot)->type == t) {
			if (t == (int)RING && (equipment.at(RING_SLOT_A) && !equipment.at(RING_SLOT_B))) {
				t++;
			}
			Object * temp = equipment.at(t-1);
			equipment.at(t-1) = inventory.at(slot);
			std::string equip_message = "You equip ";
			equip_message += inventory.at(slot)->name;
			IO_handler::queue_message(equip_message);
			inventory.at(slot) = temp;

			return true;
		}
	}
	return false;
}

bool PC::unequip_object(equipment_slots slot)
{
	if (!equipment.at(slot)) {
		return false;
	}
	for (int s = 0; s < 10; s++) {
		if (!inventory.at(s)) {
			inventory.at(s) = equipment.at(slot);
			equipment.at(slot) = NULL;
			return true;
		}
	}
	Object *temp = inventory.at(0);
	inventory.at(0) = equipment.at(slot);
	drop_item(0);
	inventory.at(0) = temp;
	return true;
}

bool PC::drop_item(int slot)
{
	if (!inventory.at(slot)) {
		return false;
	}
	if (!Dungeon::instance()->place_object(position.x, position.y, inventory.at(slot))) {
		IO_handler::queue_message("There is no space to drop that item");
		return false;
	}
	std::string drop_message = "You dropped ";
	drop_message += inventory.at(slot)->name;
	IO_handler::queue_message(drop_message);
	inventory.at(slot) = NULL;
	return true;
}

void PC::expunge_item(int slot)
{
	std::string message = "You destroyed ";
	message += inventory.at(slot)->name;
	IO_handler::queue_message(message);
	inventory.at(slot) = NULL;
}

Object * PC::inventory_object(int slot)
{
	return inventory.at(slot);
}

Object * PC::equipped_item(equipment_slots slot)
{
	return equipment.at(slot);
}

void PC::tgm()
{
	god_mode = !god_mode;
}

//helper function for update_vision
int square_dist(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	return (dx * dx) + (dy * dy);
}

void PC::update_vision(Floor *f)
{
	

	int vbox_left = this->x_pos() - vision_range;
	if (vbox_left < 0) { vbox_left = 0; }

	int vbox_right = this->x_pos() + vision_range;
	if (vbox_right >= FWIDTH) { vbox_right = FWIDTH-1; }

	int vbox_top = this->y_pos() - vision_range;
	if (vbox_top < 0) { vbox_top = 0; }

	int vbox_bottom = this->y_pos() + vision_range;
	if (vbox_bottom >= FHEIGHT) { vbox_bottom = FHEIGHT - 1; }

	f->reset_current_vision();

	std::array<int, 8> n = f->get_neighbors(x_pos(), y_pos());
	for (int i : n) {
		cellType c = f->get_type(linearX(i), linearY(i));
		//check for any cell that blocks the sight line
		if (c != rock_c && c != immutable_c) {
			f->view_cell(i);
		}
	}

	std::vector<int> sight_line;
	//iterate over a box that contains the entire visual range of the NPC
	for (int row = vbox_top; row <= vbox_bottom; row++) {
		for (int col = vbox_left; col <= vbox_right; col++) {
			//use manhattan distance to get the edge of a diamond
			if (std::abs(square_dist(x_pos(), y_pos(), col, row) - vision_range) <= 2 * vision_range) {
				//draw a line to each point on the diamond
				sight_line = f->bresenham_line(x_pos(), y_pos(), col, row);
				//Since my bresenham algorithm can return a reversed line, check whether the
				//sight line vector is reversed and flip it back
				if (sight_line.front() == index2d(col, row)) {
					std::reverse(sight_line.begin(), sight_line.end());
				}
				//iterate over the sight line from what should now be the PC's location
				//if a single opaque cell is encountered, skip the rest of the iteration
				for (int i : sight_line) {
					cellType c = f->get_type(linearX(i), linearY(i));
                    //check for any cell that blocks the sight line
					if (c == rock_c || c == immutable_c) {
						break;
					}
					//since we have unobstructed view, set PC vision to true
					f->view_cell(i);
				}
			}
		}
	}
}


