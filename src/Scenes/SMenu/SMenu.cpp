//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "SMenu.h"
#include "../../GameEngine/GameEngine.h"

SMenu::SMenu(GameEngine *game) : Scene(game) {
    init();
}

void SMenu::sDoAction(const Action &action) {
    switch (action.getName()) {
        case Action::Actions::QUIT:
            m_game->quit();
            break;
        case Action::Actions::MOVE_UP:
            if (action.getType() == Action::ActionTypes::START) {
                m_selectedMenuIndex = m_selectedMenuIndex <= 0 ? m_menuStrings.size() - 1 : m_selectedMenuIndex - 1;
            }
            break;
        case Action::Actions::MOVE_DOWN:
            if (action.getType() == Action::ActionTypes::START) {
                m_selectedMenuIndex = m_selectedMenuIndex >= m_menuStrings.size() - 1 ? 0 : m_selectedMenuIndex + 1;
            }
            break;
        case Action::Actions::SUBMIT:
            if (m_menuStrings[m_selectedMenuIndex] == "Quit") {
                m_game->quit();
            } else {
                // Change to play scene
                std::shared_ptr<Scene> playScene = std::make_shared<SPlay>(m_game, m_levelPaths[m_selectedMenuIndex]);
                m_game->changeScene(Scene::SceneType::PLAY, playScene, true);
            }
            break;
    }
}

void SMenu::init() {
    registerAction(sf::Keyboard::Q, Action::QUIT);
    registerAction(sf::Keyboard::Up, Action::MOVE_UP);
    registerAction(sf::Keyboard::Down, Action::MOVE_DOWN);

    registerAction(sf::Keyboard::Enter, Action::Actions::SUBMIT);

    m_menuStrings = {
            "Level 1",
            "Level 2",
            "Level 3",
            "Quit"
    };

    m_levelPaths = {
            "./resources/levels/level1.txt",
            "./resources/levels/level2.txt",
            "./resources/levels/level3.txt"
    };

    m_menuText.setCharacterSize(48);
    m_menuText.setFont(m_game->getAssets().getFont("FontPixel"));

    m_selectedMenuIndex = 0;

    loadBackground();
}

void SMenu::update() {
    m_eManager.update();
    for (auto &e: m_eManager.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            e->getComponent<CAnimation>().m_animation->update();
        }
    }
}

void SMenu::sRender() {
    m_game->getWindow().clear(sf::Color(92, 148, 252));

    for (auto &e: m_eManager.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            auto &sprite = e->getComponent<CAnimation>().m_animation->getSprite();
            auto &transform = e->getComponent<CTransform>();
            sprite.setPosition({transform.m_position.x(), transform.m_position.y()});
            sprite.setScale(transform.m_scale.x(), transform.m_scale.y());
            if (transform.m_scale.x() < 0) {
                // Hack, I don't like it
                sprite.setPosition(sprite.getPosition().x + sprite.getTextureRect().getSize().x,
                                   sprite.getPosition().y);
            }
            m_game->getWindow().draw(sprite);
        }
    }

    for (int i = 0; i < m_menuStrings.size(); i++) {
        auto &s = m_menuStrings[i];

        m_menuText.setString(s);
        m_menuText.setPosition(m_game->getWindow().getSize().x / 2 - m_menuText.getLocalBounds().getSize().x / 2,
                               i * m_menuText.getCharacterSize() + m_game->getWindow().getSize().y / 2 - m_menuStrings.size() * m_menuText.getCharacterSize() / 2);

        if (i == m_selectedMenuIndex) {
            m_menuText.setFillColor(sf::Color::Red);
        } else {
            m_menuText.setFillColor(sf::Color::White);
        }

        m_game->getWindow().draw(m_menuText);
    }

    m_game->getWindow().display();
}

void SMenu::loadBackground() {
    int gridWidth = m_game->getWindow().getSize().x / GRID_CELL_SIZE;
    int gridHeight = m_game->getWindow().getSize().y / GRID_CELL_SIZE;

    for (int i = 0; i < gridWidth; i++) {
        auto tile = m_eManager.addEntity(Entity::TILE);
        tile->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimGround"));
        tile->addComponent<CTransform>(gridToMidPixel(i, gridHeight - 1, tile));
        tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation->getSize());
    }

    auto player = m_eManager.addEntity(Entity::PLAYER);
    player->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimMarioBigIdle"));
    player->addComponent<CTransform>(gridToMidPixel(3, gridHeight - 2, player));
    player->addComponent<CBoundingBox>(player->getComponent<CAnimation>().m_animation->getSize());

    auto cloud1 = m_eManager.addEntity(Entity::DEC);
    cloud1->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimCloud1"));
    cloud1->addComponent<CTransform>(gridToMidPixel(1, 4, cloud1));
    cloud1->addComponent<CBoundingBox>(cloud1->getComponent<CAnimation>().m_animation->getSize());

    auto cloud2 = m_eManager.addEntity(Entity::DEC);
    cloud2->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimCloud3"));
    cloud2->addComponent<CTransform>(gridToMidPixel(5, 2, cloud2));
    cloud2->addComponent<CBoundingBox>(cloud1->getComponent<CAnimation>().m_animation->getSize());

    auto dec1 = m_eManager.addEntity(Entity::DEC);
    dec1->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimBush"));
    dec1->addComponent<CTransform>(gridToMidPixel(5, gridHeight - 2, dec1));
    dec1->addComponent<CBoundingBox>(cloud1->getComponent<CAnimation>().m_animation->getSize());

    auto dec2 = m_eManager.addEntity(Entity::DEC);
    dec2->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimLightBush2"));
    dec2->addComponent<CTransform>(gridToMidPixel(gridWidth - 8, gridHeight - 2, dec2));
    dec2->addComponent<CBoundingBox>(cloud1->getComponent<CAnimation>().m_animation->getSize());

    auto castle = m_eManager.addEntity(Entity::TILE);
    castle->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimCastle"));
    castle->addComponent<CTransform>(gridToMidPixel(gridWidth / 2 + 5, gridHeight - 2, castle));
    castle->addComponent<CBoundingBox>(castle->getComponent<CAnimation>().m_animation->getSize());

    auto brick = m_eManager.addEntity(Entity::TILE);
    brick->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimBrick2"));
    brick->addComponent<CTransform>(gridToMidPixel(gridWidth / 2 + 3, gridHeight - 2, brick));
    brick->addComponent<CBoundingBox>(brick->getComponent<CAnimation>().m_animation->getSize());

    auto flagPole = m_eManager.addEntity(Entity::TILE);
    flagPole->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimFlagPole"));
    flagPole->addComponent<CTransform>(gridToMidPixel(gridWidth / 2 + 3, gridHeight - 3, flagPole));
    flagPole->addComponent<CBoundingBox>(flagPole->getComponent<CAnimation>().m_animation->getSize());

    auto flag = m_eManager.addEntity(Entity::TILE);
    flag->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimFlag"));
    flag->addComponent<CTransform>(gridToMidPixel(gridWidth / 2 + 2, 3, brick));
    flag->addComponent<CBoundingBox>(flag->getComponent<CAnimation>().m_animation->getSize());
}

Vec2 SMenu::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    Vec2 gridCellBottomLeft = {gridX * GRID_CELL_SIZE, gridY * GRID_CELL_SIZE + GRID_CELL_SIZE};

    float entityWorldX = gridCellBottomLeft.x();
    float entityWorldY = gridCellBottomLeft.y() - entity->getComponent<CAnimation>().m_animation->getSize().y();

    return {entityWorldX, entityWorldY};
}
