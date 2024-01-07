//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_ENTITYMANAGER_H
#define COMP4300_ASSIGNMENT3_ENTITYMANAGER_H

#include "../Entity/Entity.h"

#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<Entity::Tags, EntityVec> EntityMap;

class EntityManager {
private:
    EntityVec m_entitiesVec;
    EntityMap m_entitiesMap;
    size_t m_totalEntities;

    EntityVec m_entitiesToAdd;

    void removeDeadEntitiesFromVec(EntityVec& vec);
public:
    EntityManager();
    void update();
    std::shared_ptr<Entity> addEntity(Entity::Tags tag);
    EntityVec& getEntities();
    EntityVec& getEntities(Entity::Tags tag);
};


#endif //COMP4300_ASSIGNMENT3_ENTITYMANAGER_H
