#include "PC.h"
#include "Floor.h"

#include <cstdlib>

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

void PC::update_vision()
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

	for (int row = vbox_top; row <= vbox_bottom; row++) {
		for (int col = vbox_left; col <= vbox_right; col++) {
			double x_dist = abs((double)col - this->x_pos());
			double y_dist = abs((double)row - this->y_pos());

			double dist = pow(x_dist, 2) + pow(y_dist, 2);
			if (dist <= vision_range) {
				current_vision[index2d(col, row)] = true;
				known_terrain[index2d(col, row)] = true; 
			}
		}
	}
}


void PC::reset_vision()
{
    current_vision.fill(false);
    known_terrain.fill(false);
}
