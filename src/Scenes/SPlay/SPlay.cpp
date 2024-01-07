//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "SPlay.h"
#include "../../GameEngine/GameEngine.h"

SPlay::SPlay(GameEngine *game, const std::string &levelPath)
        : Scene(game), m_levelPath(levelPath) {
    init();
}

void SPlay::init() {
    registerAction(sf::Keyboard::P, Action::PAUSE);
    registerAction(sf::Keyboard::Q, Action::QUIT);
    registerAction(sf::Keyboard::Escape, Action::BACK);
    registerAction(sf::Keyboard::T, Action::TOGGLE_TEXTURE);
    registerAction(sf::Keyboard::C, Action::TOGGLE_COLLISION);
    registerAction(sf::Keyboard::G, Action::TOGGLE_GRID);
    registerAction(sf::Keyboard::V, Action::TOGGLE_CAMERA_TRAP);

    registerAction(sf::Keyboard::W, Action::JUMP);
    registerAction(sf::Keyboard::A, Action::MOVE_LEFT);
    registerAction(sf::Keyboard::D, Action::MOVE_RIGHT);

    m_gridText.setCharacterSize(12);
    m_gridText.setOutlineThickness(.3f);
    m_gridText.setOutlineColor(sf::Color::White);
    m_gridText.setFillColor(sf::Color::White);
    m_gridText.setFont(m_game->getAssets().getFont("FontGrid"));

    m_gameText.setCharacterSize(16);
    m_gameText.setOutlineColor(sf::Color::White);
    m_gameText.setFillColor(sf::Color::White);
    m_gameText.setFont(m_game->getAssets().getFont("FontPixel"));

    m_camera.setCenter(m_game->getWindow().getSize().x / 2, m_game->getWindow().getSize().y / 2);
    m_camera.setSize(m_game->getWindow().getSize().x, m_game->getWindow().getSize().y);

    m_cameraTrap.setSize({CAMERA_TRAP_X * GRID_CELL_SIZE, CAMERA_TRAP_Y * GRID_CELL_SIZE});
    m_cameraTrap.setPosition({0, m_game->getWindow().getSize().y - m_cameraTrap.getSize().y});

    // For debug
    m_cameraTrap.setFillColor(sf::Color::Transparent);
    m_cameraTrap.setOutlineThickness(2);
    m_cameraTrap.setOutlineColor(sf::Color::Red);

    m_canJump = true;

    loadLevel();
}

void SPlay::sMovement() {
    // NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right

    // Player Movement
    if (m_player->hasComponent<CInput>()) {
        auto &input = m_player->getComponent<CInput>();
        auto &transform = m_player->getComponent<CTransform>();
        auto &gravity = m_player->getComponent<CGravity>();

        if (input.isMovingLeft) {
            // Accelerate
            transform.m_velocity.setX(transform.m_velocity.x() + (-PLAYER_X_ACCELERATION) < -PLAYER_MAX_X_VELOCITY
                                      ? -PLAYER_MAX_X_VELOCITY : transform.m_velocity.x() + (-PLAYER_X_ACCELERATION));
        } else if (transform.m_velocity.x() < 0) {
            // Decelerate
            transform.m_velocity.setX(
                    transform.m_velocity.x() + 2 * PLAYER_X_ACCELERATION > 0 ? 0 : transform.m_velocity.x() +
                                                                                   2 * PLAYER_X_ACCELERATION);
        }

        if (input.isMovingRight) {
            // Accelerate
            transform.m_velocity.setX(
                    transform.m_velocity.x() + PLAYER_X_ACCELERATION > PLAYER_MAX_X_VELOCITY ? PLAYER_MAX_X_VELOCITY :
                    transform.m_velocity.x() + PLAYER_X_ACCELERATION);
        } else if (transform.m_velocity.x() > 0) {
            // Decelerate
            transform.m_velocity.setX(
                    transform.m_velocity.x() + 2 * (-PLAYER_X_ACCELERATION) < 0 ? 0 : transform.m_velocity.x() +
                                                                                      2 * (-PLAYER_X_ACCELERATION));
        }

        if (input.isJumping && m_canJump) {
            // Accelerate
            transform.m_velocity.setY(
                    transform.m_velocity.y() + (-PLAYER_Y_ACCELERATION) < (-PLAYER_MAX_Y_VELOCITY)
                    ? -PLAYER_MAX_Y_VELOCITY :
                    transform.m_velocity.y() + (-PLAYER_Y_ACCELERATION));

            if (transform.m_position.y() <= m_startJumpPosition - PLAYER_MAX_JUMP_HEIGHT) {
                m_canJump = false;
                transform.m_velocity.setY(0.0f);
            }
        } else {
            transform.m_velocity.setY(
                    transform.m_velocity.y() + gravity.power > PLAYER_MAX_Y_VELOCITY ? PLAYER_MAX_Y_VELOCITY :
                    transform.m_velocity.y() + gravity.power);
        }


        // A bit of a hack but basically ensures that the entity cannot pass "over" another entity
        // when it's velocity it s bigger than the size of entity's B colliding box
        if (transform.m_velocity.x() >= GRID_CELL_SIZE) {
            transform.m_velocity.setX(GRID_CELL_SIZE - 1);
        } else if (transform.m_velocity.x() <= -GRID_CELL_SIZE) {
            transform.m_velocity.setX(-GRID_CELL_SIZE + 1);
        }

        if (transform.m_velocity.y() >= GRID_CELL_SIZE) {
            transform.m_velocity.setY(GRID_CELL_SIZE - 1);
        } else if (transform.m_velocity.y() <= -GRID_CELL_SIZE) {
            transform.m_velocity.setY(-GRID_CELL_SIZE + 1);
        }

        transform.m_prevPosition = transform.m_position;
        transform.m_position += transform.m_velocity;
    }

    // Other entities (enemies)
    for (auto &e: m_eManager.getEntities()) {
        // Only move entities horizontally if camera sees them
        if (e->getComponent<CTransform>().m_position.x() < m_camera.getCenter().x - m_camera.getSize().x / 2 &&
            e->getComponent<CTransform>().m_position.x() > m_camera.getSize().x) {
            continue;
        }

        if (!e->hasComponent<CBoundingBox>() || (e->getTag() != Entity::ENEMY && e->getTag() != Entity::MUSHROOM)) {
            continue;
        }

        auto &transform = e->getComponent<CTransform>();
        auto &gravity = e->getComponent<CGravity>();

        transform.m_velocity.setY(transform.m_velocity.y() + gravity.power);

        transform.m_prevPosition = transform.m_position;
        transform.m_position += transform.m_velocity;
    }
}

void SPlay::sLifespan() {
    for (auto &e: m_eManager.getEntities()) {
        if (!e->hasComponent<CLifespan>()) {
            continue;
        }

        if (e->getComponent<CLifespan>().m_frames <= 0) {
            e->destroy();
        }

        e->getComponent<CLifespan>().m_frames--;
    }
}

void SPlay::sCollision() {
    // Enemies collisions
    for (auto &eA: m_eManager.getEntities()) {
        if (eA->getTag() != Entity::ENEMY && eA->getTag() != Entity::MUSHROOM) {
            continue;
        }
        for (auto &eB: m_eManager.getEntities()) {
            if (eB->getId() == eA->getId() || eB->getTag() == Entity::PLAYER || !eB->hasComponent<CBoundingBox>()) {
                continue;
            }

            Vec2 overlap = Physics::GetOverlap(eA, eB);
            while (overlap.x() > 0 && overlap.y() > 0) { // Hack using a while. I don't understand why tbh
                // Collision resolution
                Vec2 previousOverlap = Physics::GetPreiovusOverlap(eA, eB);
                auto &eATransform = eA->getComponent<CTransform>();
                auto &eBTransform = eB->getComponent<CTransform>();
                if (previousOverlap.x() > 0 && eATransform.m_prevPosition.y() < eBTransform.m_prevPosition.y()) {
                    // eA coming from top
                    eATransform.m_position.setY(eATransform.m_position.y() - overlap.y());
                    eATransform.m_velocity.setY(0.0f);
                } else if (previousOverlap.x() > 0 && eATransform.m_prevPosition.y() > eBTransform.m_prevPosition.y()) {
                    // eA coming from bottom
                    eATransform.m_position.setY(eATransform.m_position.y() + overlap.y());
                    eATransform.m_velocity.setY(0.0f);
                } else if (previousOverlap.y() > 0 && eATransform.m_prevPosition.x() < eBTransform.m_prevPosition.x()) {
                    // eA coming from left
                    eATransform.m_position.setX(eATransform.m_position.x() - overlap.x());
                    eATransform.m_velocity.setX(-eATransform.m_velocity.x());
                } else if (previousOverlap.y() > 0 && eATransform.m_prevPosition.x() > eBTransform.m_prevPosition.x()) {
                    // eA coming from right
                    eATransform.m_position.setX(eATransform.m_position.x() + overlap.x());
                    eATransform.m_velocity.setX(-eATransform.m_velocity.x());
                } else {
                    eATransform.m_position.setY(eATransform.m_position.y() - overlap.y());
                    eATransform.m_velocity.setY(0.0f);
                }

                overlap = Physics::GetOverlap(eA, eB);
            }
        }
    }

    // Player collision with screen/view borders
    if (m_player->getComponent<CTransform>().m_position.x() <= m_camera.getCenter().x - m_camera.getSize().x / 2) {
        m_player->getComponent<CTransform>().m_position.setX(m_camera.getCenter().x - m_camera.getSize().x / 2);
    }

    if (m_player->getComponent<CTransform>().m_position.y() + m_player->getComponent<CBoundingBox>().m_size.y() >=
        m_game->getWindow().getSize().y) {
        m_player->destroy();
        spawnPlayer();
    }

    // Player collision with other entities
    for (auto &e: m_eManager.getEntities()) {
        if (!e->hasComponent<CBoundingBox>() || e->getTag() == Entity::PLAYER) {
            continue;
        }

        bool shouldSpawnCoin = false;
        bool shouldSpawnMushroom = false;
        bool shouldDestroyBrick = false;
        bool shouldKillPlayer = false;

        Vec2 overlap = Physics::GetOverlap(m_player, e);
        while (overlap.x() > 0 && overlap.y() > 0) { // Hack using a while. I don't understand why tbh
            if (e->getTag() == Entity::MUSHROOM) {
                e->destroy();
                m_playerBig = true;
                break;
            }

            // Collision resolution
            Vec2 previousOverlap = Physics::GetPreiovusOverlap(m_player, e);
            auto &pTransform = m_player->getComponent<CTransform>();
            auto &eTransform = e->getComponent<CTransform>();
            if (previousOverlap.x() > 0 && pTransform.m_prevPosition.y() < eTransform.m_prevPosition.y()) {
                // player coming from top
                pTransform.m_position.setY(pTransform.m_position.y() - overlap.y());
                pTransform.m_velocity.setY(0.0f);

                m_canJump = true;

                if (e->getTag() == Entity::ENEMY) {
                    eTransform.m_velocity.setX(0);
                    eTransform.m_velocity.setY(0);
                    e->removeComponent<CBoundingBox>();
                    if (m_player->hasComponent<CScore>()) {
                        m_player->getComponent<CScore>().m_score += 200;
                    }
                    if (e->getComponent<CAnimation>().m_animation->getName() == "AnimGoomba") {
                        Animation a = *m_game->getAssets().getAnimation("AnimGoombaDie");
                        e->getComponent<CAnimation>().m_animation = std::make_shared<Animation>(a);
                    } else if (e->getComponent<CAnimation>().m_animation->getName() == "AnimKoopa") {
                        Animation a = *m_game->getAssets().getAnimation("AnimKoopaDie");;
                        e->getComponent<CAnimation>().m_animation = std::make_shared<Animation>(a);
                    }
                }
            } else if (previousOverlap.x() > 0 && pTransform.m_prevPosition.y() > eTransform.m_prevPosition.y()) {
                // player coming from bottom
                pTransform.m_position.setY(pTransform.m_position.y() + overlap.y());
                pTransform.m_velocity.setY(0.0f);

                m_canJump = false;
                if (e->getTag() == Entity::ENEMY) {
                    shouldKillPlayer = true;
                }

                if (e->hasComponent<CAnimation>()) {
                    if (e->getComponent<CAnimation>().m_animation->getName() == "AnimCoinBrick") {
                        shouldSpawnCoin = true;
                    } else if (e->getComponent<CAnimation>().m_animation->getName() == "AnimMushroomBrick") {
                        shouldSpawnMushroom = true;
                    }

                    if (e->getComponent<CAnimation>().m_animation->getName() == "AnimBrick" && m_playerBig) {
                        shouldDestroyBrick = true;
                    }
                }

            } else if (previousOverlap.y() > 0 && pTransform.m_prevPosition.x() < eTransform.m_prevPosition.x()) {
                // player coming from left
                pTransform.m_position.setX(pTransform.m_position.x() - overlap.x());

                if (e->getTag() == Entity::ENEMY) {
                    shouldKillPlayer = true;
                }
            } else if (previousOverlap.y() > 0 && pTransform.m_prevPosition.x() > eTransform.m_prevPosition.x()) {
                // player coming from right
                pTransform.m_position.setX(pTransform.m_position.x() + overlap.x());

                if (e->getTag() == Entity::ENEMY) {
                    shouldKillPlayer = true;
                }
            } else {
                pTransform.m_position.setY(pTransform.m_position.y() - overlap.y());
                pTransform.m_velocity.setY(0.0f);
            }

            overlap = Physics::GetOverlap(m_player, e);
        }

        if (shouldSpawnCoin) {
            auto &eBTransform = e->getComponent<CTransform>();
            auto &eBBBox = e->getComponent<CBoundingBox>();

            spawnCoin({eBTransform.m_position.x(),
                       eBTransform.m_position.y() - eBBBox.m_size.y()}); // This is very hard coded ..


            e->getComponent<CAnimation>().m_animation = m_game->getAssets().getAnimation("AnimHitBrick");
        }

        if (shouldSpawnMushroom) {
            auto &eBTransform = e->getComponent<CTransform>();
            auto &eBBBox = e->getComponent<CBoundingBox>();

            spawnMushroom({eBTransform.m_position.x(),
                           eBTransform.m_position.y() - eBBBox.m_size.y()});

            e->getComponent<CAnimation>().m_animation = m_game->getAssets().getAnimation("AnimHitBrick");
        }

        if (shouldKillPlayer) {
            if (m_playerBig) {
                m_playerBig = false;
            } else {
                spawnPlayer(true);
            }
        }

        if (shouldDestroyBrick) {
            e->destroy();
        }
    }
}

void SPlay::loadLevel() {
    // Reset the entity manager every time we load a level
    m_eManager = EntityManager();

    std::ifstream file(m_levelPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open level file " << m_levelPath << std::endl;
        exit(-1);
    }

    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> tokens = m_game->splitString(line, ' ');
        if (tokens[0] == "Tile") {
            auto tile = m_eManager.addEntity(Entity::TILE);
            std::string &animationName = tokens[1];
            int gridX = std::atoi(tokens[2].c_str());
            int gridY = std::atoi(tokens[3].c_str());
            auto &anim = m_game->getAssets().getAnimation(animationName);

            tile->addComponent<CAnimation>(anim);
            tile->addComponent<CBoundingBox>(Vec2(anim->getSize().x(), anim->getSize().y()));
            tile->addComponent<CTransform>(gridToMidPixel(gridX, gridY, tile));
            tile->getComponent<CTransform>().m_prevPosition = tile->getComponent<CTransform>().m_position;
        }

        if (tokens[0] == "Dec") {
            auto dec = m_eManager.addEntity(Entity::TILE);
            std::string &animationName = tokens[1];
            int gridX = std::atoi(tokens[2].c_str());
            int gridY = std::atoi(tokens[3].c_str());
            auto &anim = m_game->getAssets().getAnimation(animationName);

            dec->addComponent<CAnimation>(anim);
            dec->addComponent<CTransform>(gridToMidPixel(gridX, gridY, dec));
            dec->getComponent<CTransform>().m_prevPosition = dec->getComponent<CTransform>().m_position;

            // Hack, I don't like but I'm too tired to mess around in photoshop right now
            if (animationName == "AnimFlag") {
                dec->getComponent<CTransform>().m_position.setX(dec->getComponent<CTransform>().m_position.x() - 28);
            }
        }

        if (tokens[0] == "Player") {
            // TODO: Add the animation for bullet
            m_player = m_eManager.addEntity(Entity::PLAYER);
            m_playerConfig.X = std::atof(tokens[1].c_str());
            m_playerConfig.Y = std::atof(tokens[2].c_str());
            m_playerConfig.CX = std::atof(tokens[3].c_str());
            m_playerConfig.CY = std::atof(tokens[4].c_str());
            m_playerConfig.JUMP = std::atof(tokens[6].c_str());
            m_playerConfig.SPEED = std::atof(tokens[7].c_str());
            m_playerConfig.GRAVITY = std::atof(tokens[8].c_str());
        }

        if (tokens[0] == "Enemy") {
            auto enemy = m_eManager.addEntity(Entity::ENEMY);
            std::string &animationName = tokens[1];
            int gridX = std::atoi(tokens[2].c_str());
            int gridY = std::atoi(tokens[3].c_str());
            int direction = std::atoi(tokens[4].c_str());
            float speed = std::atof(tokens[5].c_str());
            auto &anim = m_game->getAssets().getAnimation(animationName);

            enemy->addComponent<CAnimation>(anim);
            enemy->addComponent<CBoundingBox>(Vec2(anim->getSize().x(), anim->getSize().y()));
            enemy->addComponent<CTransform>(gridToMidPixel(gridX, gridY, enemy));
            enemy->getComponent<CTransform>().m_velocity.setX(speed * direction);
            enemy->getComponent<CTransform>().m_prevPosition = enemy->getComponent<CTransform>().m_position;
            enemy->addComponent<CGravity>();
        }

        if (tokens[0] == "Level") {
            m_levelTime = std::atoi(tokens[1].c_str());
        }
    }
    file.close();

    spawnPlayer();
}

void SPlay::update() {
    m_eManager.update();
    m_currentFrame++;

    if (m_currentFrame >= 60) {
        // Not really a seconds counter but meh..
        m_levelTime--;
        m_currentFrame = 0;
    }

    if (m_levelTime == 0) {
        m_gameOver = true;
    }

    if (m_gameOver) {
        // TODO: Implement game over routine based on whether the player won or lost
    }

    if (!m_paused) {
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
    }

    sCamera();
    sRender();
}

void SPlay::sRender() {
    m_game->getWindow().clear(sf::Color(92, 148, 252));
    m_game->getWindow().setView(m_camera);

    for (auto &e: m_eManager.getEntities()) {
        if (m_drawTextures && e->hasComponent<CTransform>() && e->hasComponent<CAnimation>()) {
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

        if (m_drawCollision && e->hasComponent<CTransform>() && e->hasComponent<CBoundingBox>()) {
            sf::RectangleShape rect;
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineThickness(1.f);
            rect.setOutlineColor(sf::Color::Green);

            auto &bbox = e->getComponent<CBoundingBox>();
            auto &transform = e->getComponent<CTransform>();
            rect.setPosition({(float) transform.m_position.x(), (float) transform.m_position.y()});
            rect.setSize({(float) bbox.m_size.x(), (float) bbox.m_size.y()});

            m_game->getWindow().draw(rect);
        }
    }

    if (m_drawCameraTrap) {
        m_game->getWindow().draw(m_cameraTrap);
    }

    if (m_drawGrid) {
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineThickness(.5f);
        rect.setOutlineColor(sf::Color::White);
        rect.setSize({GRID_CELL_SIZE, GRID_CELL_SIZE});

        int gridWidth = m_camera.getSize().x / GRID_CELL_SIZE;
        int gridHeight = m_camera.getSize().y / GRID_CELL_SIZE;
        for (int i = 0; i < gridHeight; i++) {
            for (int j = 0; j < gridWidth + 1; j++) {

                int overflowCellsX = (int) ((m_camera.getCenter().x - m_camera.getSize().x / 2) / GRID_CELL_SIZE);
                int overflowCellsY = (int) ((m_camera.getCenter().y - m_camera.getSize().y / 2) / GRID_CELL_SIZE);

                float cellX = overflowCellsX * GRID_CELL_SIZE + j * GRID_CELL_SIZE;
                float cellY = overflowCellsY * GRID_CELL_SIZE + i * GRID_CELL_SIZE;

                rect.setPosition({cellX, cellY});
                m_game->getWindow().draw(rect);

                m_gridText.setString(std::to_string(overflowCellsX + j) + ", " + std::to_string(overflowCellsY + i));
                m_gridText.setPosition(
                        {rect.getPosition().x + rect.getSize().x - m_gridText.getGlobalBounds().width - 2,
                         rect.getPosition().y + rect.getSize().y - m_gridText.getCharacterSize() - 5});
                m_game->getWindow().draw(m_gridText);
            }
        }
    }


    m_gridText.setFillColor(sf::Color::Red);
    m_gridText.setPosition({m_camera.getCenter().x + m_camera.getSize().x / 2 - 250.f, 30.f});
    m_gridText.setString("Press G - Toggle [G]rid");
    m_game->getWindow().draw(m_gridText);

    m_gridText.setPosition(
            {m_camera.getCenter().x + m_camera.getSize().x / 2 - 250.f, 1 * m_gridText.getCharacterSize() + 30.f});
    m_gridText.setString("Press T - Toggle [T]extures");
    m_game->getWindow().draw(m_gridText);

    m_gridText.setPosition(
            {m_camera.getCenter().x + m_camera.getSize().x / 2 - 250.f, 2 * m_gridText.getCharacterSize() + 30.f});
    m_gridText.setString("Press C - Toggle [C]ollisions");
    m_game->getWindow().draw(m_gridText);

    m_gridText.setPosition(
            {m_camera.getCenter().x + m_camera.getSize().x / 2 - 250.f, 3 * m_gridText.getCharacterSize() + 30.f});
    m_gridText.setString("Press V - Toggle Camera Trap");
    m_game->getWindow().draw(m_gridText);

    m_gridText.setPosition(
            {m_camera.getCenter().x + m_camera.getSize().x / 2 - 250.f, 4 * m_gridText.getCharacterSize() + 30.f});
    m_gridText.setString("Press P - [P]ause");
    m_game->getWindow().draw(m_gridText);


    if (m_player->hasComponent<CScore>()) {
        m_gameText.setCharacterSize(16);
        m_gameText.setString("Score");
        m_gameText.setPosition(m_camera.getCenter().x - m_camera.getSize().x / 2 + 5, m_camera.getCenter().y - m_camera.getSize().y / 2 + 5);
        m_game->getWindow().draw(m_gameText);

        m_gameText.setString(std::string("0000" + std::to_string(m_player->getComponent<CScore>().m_score)));
        m_gameText.setPosition(m_camera.getCenter().x - m_camera.getSize().x / 2 + 5, m_camera.getCenter().y - m_camera.getSize().y / 2 + 5 + m_gameText.getCharacterSize());
        m_game->getWindow().draw(m_gameText);
    }

    m_gameText.setCharacterSize(16);
    m_gameText.setString("Time");
    m_gameText.setPosition(m_camera.getCenter().x - m_gameText.getLocalBounds().width / 2,
                           m_camera.getCenter().y - m_camera.getSize().y / 2 + 5);
    m_game->getWindow().draw(m_gameText);

    m_gameText.setString(std::to_string(m_levelTime));
    m_gameText.setPosition(m_camera.getCenter().x - m_gameText.getLocalBounds().width / 2,
                           m_camera.getCenter().y - m_camera.getSize().y / 2 + 5 + m_gameText.getCharacterSize());
    m_game->getWindow().draw(m_gameText);

    if (m_player->hasComponent<CLives>()) {
        m_gameText.setCharacterSize(16);
        m_gameText.setString("Lives");
        m_gameText.setPosition(m_camera.getCenter().x + m_camera.getSize().x / 2 - m_gameText.getLocalBounds().width - 15,
                               m_camera.getCenter().y - m_camera.getSize().y / 2 + 5);
        m_game->getWindow().draw(m_gameText);

        m_gameText.setPosition(m_camera.getCenter().x + m_camera.getSize().x / 2 - m_gameText.getLocalBounds().width / 2 - 15,
                               m_camera.getCenter().y - m_camera.getSize().y / 2 + 5 + m_gameText.getCharacterSize());
        m_gameText.setString(std::to_string(m_player->getComponent<CLives>().m_lives));
        m_game->getWindow().draw(m_gameText);
    }

    if (m_paused) {
        m_gameText.setString("PAUSED");
        m_gameText.setCharacterSize(24);
        m_gameText.setPosition(m_camera.getCenter().x - m_gameText.getLocalBounds().width / 2, m_camera.getCenter().y - m_gameText.getCharacterSize() / 2);
        m_game->getWindow().draw(m_gameText);
    }

    m_game->getWindow().display();
}

void SPlay::sDoAction(const Action &action) {
    if (action.getType() == Action::ActionTypes::START) {
        switch (action.getName()) {
            case Action::Actions::QUIT:
                m_game->quit();
                break;
            case Action::Actions::TOGGLE_GRID:
                m_drawGrid = !m_drawGrid;
                break;
            case Action::Actions::TOGGLE_COLLISION:
                m_drawCollision = !m_drawCollision;
                break;
            case Action::Actions::TOGGLE_TEXTURE:
                m_drawTextures = !m_drawTextures;
                break;
            case Action::Actions::TOGGLE_CAMERA_TRAP:
                m_drawCameraTrap = !m_drawCameraTrap;
                break;
            case Action::Actions::BACK:
                m_game->changeScene(Scene::MENU, std::make_shared<SMenu>(m_game));
                break;
            case Action::Actions::JUMP:
                if (m_player->hasComponent<CInput>() && m_canJump) {
                    m_player->getComponent<CInput>().isJumping = true;
                    m_canJump = false;
                    m_startJumpPosition = m_player->getComponent<CTransform>().m_position.y();
                }
                break;
            case Action::Actions::MOVE_RIGHT:
                if (m_player->hasComponent<CInput>()) {
                    m_player->getComponent<CInput>().isMovingRight = true;
                }
                if (m_player->hasComponent<CState>()) {
                    m_player->getComponent<CState>().m_state = CState::WALKING;
                }
                if (m_player->hasComponent<CTransform>()) {
                    m_player->getComponent<CTransform>().m_scale.setX(1);
                }
                break;
            case Action::Actions::MOVE_LEFT:
                if (m_player->hasComponent<CInput>()) {
                    m_player->getComponent<CInput>().isMovingLeft = true;
                }
                if (m_player->hasComponent<CState>()) {
                    m_player->getComponent<CState>().m_state = CState::WALKING;
                }
                if (m_player->hasComponent<CTransform>()) {
                    m_player->getComponent<CTransform>().m_scale.setX(-1);
                }
                break;
            case Action::Actions::PAUSE:
                m_paused = !m_paused;
                break;
        }
    }

    if (action.getType() == Action::ActionTypes::STOP) {
        switch (action.getName()) {
            case Action::Actions::JUMP:
                if (m_player->hasComponent<CInput>()) {
                    m_player->getComponent<CInput>().isJumping = false;
                }
                break;
            case Action::Actions::MOVE_RIGHT:
                if (m_player->hasComponent<CInput>()) {
                    m_player->getComponent<CInput>().isMovingRight = false;
                }
                if (m_player->hasComponent<CState>()) {
                    m_player->getComponent<CState>().m_state = CState::IDLE;
                }
                break;
            case Action::Actions::MOVE_LEFT:
                if (m_player->hasComponent<CInput>()) {
                    m_player->getComponent<CInput>().isMovingLeft = false;
                }
                if (m_player->hasComponent<CState>()) {
                    m_player->getComponent<CState>().m_state = CState::IDLE;
                }
                break;
        }
    }
}

void SPlay::spawnPlayer(bool killCurrent) {
    int score = 0;
    int lives = 3;

    if (killCurrent && m_player) {
        m_player->destroy();
        score = m_player->getComponent<CScore>().m_score;
        lives = m_player->getComponent<CLives>().m_lives - 1;
    }

    if (lives <= 0) {
        m_gameOver = true;
        return;
    }

    m_player = m_eManager.addEntity(Entity::PLAYER);

    auto &anim = m_game->getAssets().getAnimation("AnimMarioMove");
    m_player->addComponent<CAnimation>(anim);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
    m_player->addComponent<CInput>();
    m_player->addComponent<CGravity>();
    m_player->addComponent<CState>(CState::IDLE);
    m_player->addComponent<CScore>(score);
    m_player->addComponent<CLives>(lives);

    m_camera.setCenter(m_game->getWindow().getSize().x / 2, m_game->getWindow().getSize().y / 2);
    m_camera.setSize(m_game->getWindow().getSize().x, m_game->getWindow().getSize().y);

    m_cameraTrap.setSize({CAMERA_TRAP_X * GRID_CELL_SIZE, CAMERA_TRAP_Y * GRID_CELL_SIZE});
    m_cameraTrap.setPosition({0, m_game->getWindow().getSize().y - m_cameraTrap.getSize().y});
}

Vec2 SPlay::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    Vec2 gridCellBottomLeft = {gridX * GRID_CELL_SIZE, gridY * GRID_CELL_SIZE + GRID_CELL_SIZE};

    float entityWorldX = gridCellBottomLeft.x();
    float entityWorldY = gridCellBottomLeft.y() - entity->getComponent<CAnimation>().m_animation->getSize().y();

    return {entityWorldX, entityWorldY};
}

void SPlay::spawnBullet(std::shared_ptr<Entity> entity) {
    // TODO: spawn a bullet at the given entity, going in the direction the entity is facing
}

void SPlay::sAnimation() {
    if (m_player->hasComponent<CState>()) {
        std::string animIdle = m_playerBig ? "AnimMarioBigIdle" : "AnimMarioIdle";
        std::string animMove = m_playerBig ? "AnimMarioBigMove" : "AnimMarioMove";

        switch (m_player->getComponent<CState>().m_state) {
            case CState::IDLE:
                m_player->getComponent<CAnimation>().m_animation = m_game->getAssets().getAnimation(animIdle);;
                break;
            case CState::WALKING:
                m_player->getComponent<CAnimation>().m_animation = m_game->getAssets().getAnimation(animMove);;
                break;
        }
    }

    // This should reside somewhere else tbh
    m_player->getComponent<CBoundingBox>().m_size = m_player->getComponent<CAnimation>().m_animation->getSize();
    m_player->getComponent<CBoundingBox>().m_halfSize = m_player->getComponent<CBoundingBox>().m_size / 2;


    for (auto &e: m_eManager.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            if (e->getComponent<CAnimation>().m_animation->hasEnded()) {
                e->destroy();
                continue;
            }
            e->getComponent<CAnimation>().m_animation->update();
        }
    }
}

void SPlay::sCamera() {
    if (m_player && m_player->hasComponent<CTransform>() && m_player->hasComponent<CBoundingBox>()) {
        auto &pT = m_player->getComponent<CTransform>();
        auto &pB = m_player->getComponent<CBoundingBox>();

        Vec2 delta{
                pT.m_position.x() + pB.m_size.x() - (m_cameraTrap.getPosition().x + m_cameraTrap.getSize().x),
                pT.m_position.y() - m_cameraTrap.getPosition().y
        };

        if (delta.x() > 0) {
            m_camera.move({delta.x(), 0});
            m_cameraTrap.move({delta.x(), 0});
        }

        if (delta.y() < 0) {
            m_camera.move(0, delta.y());
            m_cameraTrap.move({0, delta.y()});
        }

        float playerLower = pT.m_position.y() + pB.m_size.y() + GRID_CELL_SIZE;
        float cameraLower = m_camera.getCenter().y + m_camera.getSize().y / 2;

        float deltaLower = playerLower - cameraLower;
        if (deltaLower > 0) {
            m_camera.move(0, deltaLower);
            m_cameraTrap.move({0, deltaLower});
        }
    }

}

void SPlay::spawnCoin(Vec2 position) {
    auto coin = m_eManager.addEntity(Entity::Tags::TILE);
    coin->addComponent<CTransform>(position);
    coin->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimCoin"));
    coin->addComponent<CLifespan>(30);

    if (m_player->hasComponent<CScore>()) {
        m_player->getComponent<CScore>().m_score += 100;
    }
}

void SPlay::spawnMushroom(Vec2 position) {
    auto mushroom = m_eManager.addEntity(Entity::Tags::MUSHROOM);
    mushroom->addComponent<CTransform>(position);
    mushroom->getComponent<CTransform>().m_velocity.setX(2.f);
    mushroom->addComponent<CAnimation>(m_game->getAssets().getAnimation("AnimMushroom"));
    auto &anim = mushroom->getComponent<CAnimation>().m_animation;
    mushroom->addComponent<CBoundingBox>(Vec2(anim->getSize().x(), anim->getSize().y()));
}
