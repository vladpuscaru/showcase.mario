//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_SMENU_H
#define COMP4300_ASSIGNMENT3_SMENU_H

#include "../Scene/Scene.h"
#include <SFML/Graphics.hpp>

class SMenu : public Scene {
private:

    std::string m_title;
    std::vector<std::string> m_menuStrings;
    std::vector<std::string> m_levelPaths;
    sf::Text m_menuText;
    size_t m_selectedMenuIndex;

    void init();
    void loadBackground();
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:

    explicit SMenu(GameEngine* game);

    void update() override;
    void sRender() override;
    void sDoAction(const Action& action) override;
};


#endif //COMP4300_ASSIGNMENT3_SMENU_H
