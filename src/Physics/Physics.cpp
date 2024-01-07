//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Physics.h"

// positive overlap in both dimensions means there is a collision
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB) {
    auto& bA = entityA->getComponent<CBoundingBox>();
    auto& tA = entityA->getComponent<CTransform>();

    auto& bB = entityB->getComponent<CBoundingBox>();
    auto& tB = entityB->getComponent<CTransform>();

    Vec2 centerA = tA.m_position + bA.m_halfSize;
    Vec2 centerB = tB.m_position + bB.m_halfSize;

    Vec2 delta = {
            std::abs(centerA.x() - centerB.x()),
            std::abs(centerA.y() - centerB.y())
    };

    return bA.m_halfSize + bB.m_halfSize - delta;
}

Vec2 Physics::GetPreiovusOverlap(std::shared_ptr<Entity> entityA, std::shared_ptr<Entity> entityB) {
    auto& bA = entityA->getComponent<CBoundingBox>();
    auto& tA = entityA->getComponent<CTransform>();

    auto& bB = entityB->getComponent<CBoundingBox>();
    auto& tB = entityB->getComponent<CTransform>();

    Vec2 centerA = tA.m_prevPosition + bA.m_halfSize;
    Vec2 centerB = tB.m_prevPosition + bB.m_halfSize;

    Vec2 delta = {
            std::abs(centerA.x() - centerB.x()),
            std::abs(centerA.y() - centerB.y())
    };

    return bA.m_halfSize + bB.m_halfSize - delta;
}
