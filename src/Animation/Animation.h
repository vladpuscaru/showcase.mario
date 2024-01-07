//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_ANIMATION_H
#define COMP4300_ASSIGNMENT3_ANIMATION_H

#include <SFML/Graphics.hpp>
#include "../Vec2/Vec2.h"

class Animation {
public:
    Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed, bool loop);

    void update();
    bool hasEnded() const;
    const Vec2& getSize();
    sf::Sprite& getSprite();

    const std::string& getName() const;

private:
    sf::Sprite m_sprite;
    std::string m_name;

    size_t m_frameCount; // Total number of Frames of Animation
    size_t m_currentFrame; // window frames since the animation is playing

    size_t m_speed; // The speed at which to play this animation
    Vec2 m_size; // Size of an animation frame

    bool m_loop;
};


#endif //COMP4300_ASSIGNMENT3_ANIMATION_H
