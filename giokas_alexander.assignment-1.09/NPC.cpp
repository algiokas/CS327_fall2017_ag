#include "NPC.h"
#include "Floor.h"

NPC::~NPC()
{
}

int NPC::get_next_location(Floor *f)
{
	return 0;
}

int NPC::smart_next(Floor * f)
{
	return 0;
}

int NPC::erratic_next(Floor *f)
{
	std::array<int, 8> n = f->get_neighbors(this->x_pos(), this->y_pos());
	int rand_dir = rand() % 8;
	return n[rand_dir];
}

int NPC::tele_next(Floor * f)
{
	return 0;
}

int NPC::tunnel_next(Floor * f)
{
	return 0;
}

int NPC::pass_next(Floor * f)
{
	return 0;
}
