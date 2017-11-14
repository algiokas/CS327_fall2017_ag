#ifndef EVENT_H

#define EVENT_H


enum event_type {
	event_character_turn,
};

class Character;

class Event
{
public:
	Event();
	~Event();

	event_type get_type();
	int get_time();
	int get_sequence();
	Character *get_character();

	int compare_to(Event *other);

private:
	event_type type;
	int time;
	int sequence;
	Character *c;
};

#endif

