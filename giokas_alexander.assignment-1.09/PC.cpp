#include "PC.h"
#include "Floor.h"

#include <cstdlib>
#include <cmath>


PC::PC()
{
	sym = '@';
	x_pos = 0;
	y_pos = 0;
	spd = pc_speed;
	alive = true;
	seq_num = 0;
	vision_range = pow(pc_vision_range, 2);
}

PC::PC(int x, int y)
{
	sym = '@';
	x_pos = x;
	y_pos = y;
	spd = pc_speed;
	alive = true;
	seq_num = 0;
	vision_range = pow(pc_vision_range, 2);
}

PC::~PC()
{
}

void PC::move_to(int x, int y)
{
	this->x_pos = x;
	this->y_pos = y;
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
	int vbox_left = this->x_pos - vision_range;
	if (vbox_left < 0) { vbox_left = 0; }

	int vbox_right = this->x_pos + vision_range;
	if (vbox_right >= FWIDTH) { vbox_right = FWIDTH-1; }

	int vbox_top = this->y_pos - vision_range;
	if (vbox_top < 0) { vbox_top = 0; }

	int vbox_bottom = this->y_pos + vision_range;
	if (vbox_bottom >= FHEIGHT) { vbox_bottom = FHEIGHT - 1; }

	this->current_vision.fill(false);

	for (int row = vbox_top; row <= vbox_bottom; row++) {
		for (int col = vbox_left; col <= vbox_right; col++) {
			double x_dist = abs(col - this->x_pos);
			double y_dist = abs(row - this->y_pos);

			double dist = pow(x_dist, 2) + pow(y_dist, 2);
			if (dist <= vision_range) {
				current_vision[index2d(col, row)] = true;
				known_terrain[index2d(col, row)] = true; 
			}
		}
	}
}


