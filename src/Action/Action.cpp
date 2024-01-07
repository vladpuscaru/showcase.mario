//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Action.h"

Action::Actions Action::getName() const {
    return m_name;
}

Action::ActionTypes Action::getType() const {
    return m_type;
}

Action::Action(Action::Actions name, Action::ActionTypes type)
    : m_name(name), m_type(type)
{

}
