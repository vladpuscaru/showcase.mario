//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_PHYSICS_H
#define COMP4300_ASSIGNMENT3_PHYSICS_H

#include "../Vec2/Vec2.h"
#include "../Entities/Entity/Entity.h"

class Physics {
public:
    static Vec2 GetOverlap(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
    static Vec2 GetPreiovusOverlap(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB);
};


#endif //COMP4300_ASSIGNMENT3_PHYSICS_H
