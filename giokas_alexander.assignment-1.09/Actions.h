#ifndef ACTIONS_H

#define ACTIONS_H

enum action_type { move_action, attack_action }

class Action {
    public:
        Action(Character *c_ptr, action_type a_type) :
            actor(c_ptr), type(a_type) {}
        ~Action();
    private;
        Character *actor;
        action_type type;
}

#endif
