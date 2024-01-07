//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_ACTION_H
#define COMP4300_ASSIGNMENT3_ACTION_H


class Action {
public:

    enum Actions {
        PAUSE, QUIT, BACK, TOGGLE_TEXTURE, TOGGLE_COLLISION, TOGGLE_GRID, TOGGLE_CAMERA_TRAP,
        JUMP, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT,
        SUBMIT
    };

    enum ActionTypes {
        START, STOP
    };

    Action(Actions name, ActionTypes type);

    Actions getName() const;
    ActionTypes getType() const;
private:
    Actions m_name;
    ActionTypes m_type;
};


#endif //COMP4300_ASSIGNMENT3_ACTION_H
