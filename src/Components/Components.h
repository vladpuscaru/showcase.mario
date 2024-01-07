//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_COMPONENTS_H
#define COMP4300_ASSIGNMENT3_COMPONENTS_H

#include "../Vec2/Vec2.h"
#include "../Animation/Animation.h"

class Component {
public:
    bool has = false;
};

class CTransform : public Component {
public:
    Vec2 m_position;
    Vec2 m_prevPosition;
    Vec2 m_velocity;
    Vec2 m_scale;
    double m_rotation;

    CTransform() = default;
    CTransform(Vec2 position) : m_position(position), m_prevPosition(position), m_velocity(0, 0), m_scale(1, 1) {}
    CTransform(Vec2 position, Vec2 velocity, Vec2 scale, double rotation) : m_position(position), m_velocity(velocity), m_scale(scale), m_rotation(rotation) {}
};

class CScore : public Component {
public:
    int m_score;

    CScore() = default;
    explicit CScore(int score): m_score(score) {}
};

class CLives : public Component {
public:
    int m_lives;

    CLives() = default;
    explicit CLives(int lives): m_lives(lives) {}
};

class CBoundingBox : public Component {
public:
    Vec2 m_size;
    Vec2 m_halfSize;

    CBoundingBox() = default;
    explicit CBoundingBox(Vec2 size) : m_size(size), m_halfSize(size.x() / 2, size.y() / 2) {}
};

class CLifespan : public Component {
public:
    int m_frames = 100;

    CLifespan() = default;
    explicit CLifespan(int frames): m_frames(frames) {}
};

class CInput : public Component {
public:
    bool isMovingRight = false;
    bool isMovingLeft = false;
    bool isJumping = false;
    bool isFiring = false;
};

class CAnimation : public Component {
public:
    std::shared_ptr<Animation> m_animation;

    CAnimation() = default;
    CAnimation(std::shared_ptr<Animation> animation)
        : m_animation(animation)
    {}
};

class CState : public Component {
public:
    enum States {
        IDLE, WALKING
    };
    States m_state = IDLE;

    CState() = default;
    explicit CState(States state) : m_state(state) {}
};

class CGravity : public Component {
public:
    float power = 9.8;
};

#endif //COMP4300_ASSIGNMENT3_COMPONENTS_H
