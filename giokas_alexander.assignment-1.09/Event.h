#ifndef EVENT_H

#define EVENT_H


enum event_type {
	event_npc_turn,
	event_player_turn
};

class Character;

class Event
{
public:
	Event(event_type type, Character *c) :
		type(type), c(c) {}
	~Event();

	event_type type;
	Character *c;
};

#endif

