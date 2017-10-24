#include <stdlib.h>

#include "pc.h"
#include "dungeon.h"

int spawn_pc(struct Floor *floor) {
    int rnum = rand() % floor->numRooms;
    int pc_x = floor->rooms[rnum].loc.x + (rand() % floor->rooms[rnum].dims.x);
    int pc_y = floor->rooms[rnum].loc.y + (rand() % floor->rooms[rnum].dims.y);

    int pc_loc = INDEX2D(pc_x, pc_y);
    floor->pc_loc = pc_loc;
    return 0;
}

void pc_delete(struct PC *pc)
{
    if (pc) {
        free(pc);
    }
}

int pc_is_alive(struct Floor *floor)
{
    return 0;
}

	
