//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_ASSETS_H
#define COMP4300_ASSIGNMENT3_ASSETS_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../Animation/Animation.h"

class Assets {
private:
    std::map<const std::string, sf::Texture> m_textures;
    std::map<const std::string, std::shared_ptr<Animation>> m_animations;
    std::map<const std::string, sf::Sound> m_sounds;
    std::map<const std::string, sf::Font> m_fonts;

public:
    void addTexture(const std::string& name, const std::string& path);
    void addAnimation(const std::string& name, std::shared_ptr<Animation> animation);
    void addSound(const std::string& name, const std::string& path);
    void addFont(const std::string& name, const std::string& path);

    const sf::Texture& getTexture(const std::string& name) const;
    const std::shared_ptr<Animation>& getAnimation(const std::string& name) const;
    const sf::Sound& getSound(const std::string& name) const;
    const sf::Font& getFont(const std::string& name) const;
};


#endif //COMP4300_ASSIGNMENT3_ASSETS_H
