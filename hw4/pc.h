#ifndef PC_H

#define PC_H

struct Floor;

struct PC {};

int delete_pc(struct PC *pc);
int spawn_pc(struct Floor *floor);
int pc_is_alive(struct Floor *floor);

#endif
