//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Animation.h"

sf::Sprite &Animation::getSprite() {
    return m_sprite;
}

const Vec2 &Animation::getSize() {
    return m_size;
}

Animation::Animation(const std::string& name, const sf::Texture &texture, size_t frameCount, size_t speed, bool loop)
    : m_name(name)
    , m_sprite(texture)
    , m_frameCount(frameCount)
    , m_currentFrame(0)
    , m_speed(speed)
    , m_loop(loop)
{
    m_size = Vec2((float)texture.getSize().x / frameCount, (float)texture.getSize().y);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x(), 0, m_size.x(), m_size.y()));
}

/**
 * Updates the animation to show the next frame, depending
 * on its speed
 * Animation loops when it reaches the end
 */
void Animation::update() {
    m_currentFrame++;

    int animFrame = (m_currentFrame / m_speed) % m_frameCount;

    sf::IntRect rectangle(animFrame * m_size.x(), 0, m_size.x(), m_size.y());
    m_sprite.setTextureRect(rectangle);
}

bool Animation::hasEnded() const {
    if (m_loop) return false;
    if (m_name == "AnimGoombaDie") {

    }

    return m_currentFrame > m_speed * m_frameCount;
}

const std::string &Animation::getName() const {
    return m_name;
}
