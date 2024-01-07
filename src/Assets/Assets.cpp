//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Assets.h"

const sf::Texture &Assets::getTexture(const std::string& name) const {
    assert(m_textures.find(name) != m_textures.end());
    return m_textures.find(name)->second;
}

const std::shared_ptr<Animation> &Assets::getAnimation(const std::string& name) const {
    assert(m_animations.find(name) != m_animations.end() || std::printf("Cannot find animation: %s", name.c_str()));
    return m_animations.find(name)->second;
}

const sf::Sound &Assets::getSound(const std::string& name) const {
    assert(m_sounds.find(name) != m_sounds.end());
    return m_sounds.find(name)->second;
}

const sf::Font &Assets::getFont(const std::string& name) const {
    assert(m_fonts.find(name) != m_fonts.end());
    return m_fonts.find(name)->second;
}

void Assets::addTexture(const std::string& name, const std::string &path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Error loading texture from " << path << std::endl;
        exit(-1);
    }
    m_textures[name] = texture;
}

void Assets::addAnimation(const std::string& name, std::shared_ptr<Animation> animation) {
    m_animations[name] = animation;
}

void Assets::addSound(const std::string& name, const std::string &path) {
    sf::SoundBuffer soundBuffer;
    if (!soundBuffer.loadFromFile(path)) {
        std::cerr << "Error loading sound from " << path << std::endl;
        exit(-1);
    }
    m_sounds[name] = sf::Sound(soundBuffer);
}

void Assets::addFont(const std::string& name, const std::string &path) {
    sf::Font font;
    if (!font.loadFromFile(path)) {
        std::cerr << "Error loading font from " << path << std::endl;
        exit(-1);
    }
    m_fonts[name] = font;
}
