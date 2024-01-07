//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_ENTITY_H
#define COMP4300_ASSIGNMENT3_ENTITY_H

#include <iostream>
#include <vector>
#include <tuple>
#include "../../Components/Components.h"

typedef std::tuple<
        CTransform,
        CLifespan,
        CInput,
        CBoundingBox,
        CAnimation,
        CGravity,
        CState,
        CScore,
        CLives
> ComponentTuple;

class Entity {
public:
    enum Tags {
        PLAYER, TILE, DEC, ENEMY, MUSHROOM
    };

private:
    const size_t m_id;
    const Tags m_tag;
    bool m_alive;
    ComponentTuple m_components;

    Entity(Tags tag, size_t id);

    friend class EntityManager;
public:
    template<typename T>
    T& getComponent() {
        return std::get<T>(m_components);
    }

    template<typename T>
    const T& getComponent() const {
        return std::get<T>(m_components);
    }

    template <typename T>
    bool hasComponent() const {
        return getComponent<T>().has;
    }

    template <typename T, typename... TArgs>
    T & addComponent(TArgs&&... mArgs) {
        auto& component = getComponent<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.has = true;
        return component;
    }

    template<typename T>
    void removeComponent() {
        getComponent<T>() = T(); // has value of new Component is false by default
    }


    Tags getTag() const;
    size_t getId() const;
    bool isAlive() const;
    void destroy();
};

#endif //COMP4300_ASSIGNMENT3_ENTITY_H