#include "PC.h"
#include "Floor.h"

#include <cstdlib>
#include <cmath>

PC::~PC()
{
}

bool PC::can_see(int x, int y)
{
	return current_vision[index2d(x, y)];
}

bool PC::has_seen(int x, int y)
{
	return known_terrain[index2d(x, y)];
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

	this->current_vision.fill(false);

	std::vector<int> sight_line;
	//iterate over a box that contains the entire visual range of the NPC
	for (int row = vbox_top; row <= vbox_bottom; row++) {
		for (int col = vbox_left; col <= vbox_right; col++) {
			//use manhattan distance to get the edge of a diamond
			if (std::abs(x_pos() - col) + std::abs(y_pos() - row) == vision_range) {
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
					if (c == rock_c || c == immutable_c) {
						break;
					}
					//since we have unobstructed view, set PC vision to true
					current_vision[i] = true;
					known_terrain[i] = true;
				}
			}
		}
	}
}

void PC::reset_vision()
{
    current_vision.fill(false);
    known_terrain.fill(false);
}
