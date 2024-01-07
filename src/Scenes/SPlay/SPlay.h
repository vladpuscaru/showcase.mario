//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_SPLAY_H
#define COMP4300_ASSIGNMENT3_SPLAY_H

#include "../Scene/Scene.h"
#include "../../Physics/Physics.h"
#include <fstream>

#define GRID_CELL_SIZE 64

#define PLAYER_X_ACCELERATION .5
#define PLAYER_MAX_X_VELOCITY 12.5

#define PLAYER_Y_ACCELERATION 9.8
#define PLAYER_MAX_Y_VELOCITY 68
#define PLAYER_MAX_JUMP_HEIGHT 220 // in pixels

#define CAMERA_TRAP_X 5 // distance from left (in cells)
#define CAMERA_TRAP_Y 7 // distance from top (in cells)

class SPlay : public Scene {
private:
    struct PlayerConfig {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

    int m_currentFrame = 0;

    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;

    sf::View m_camera;
    sf::RectangleShape m_cameraTrap;

    bool m_playerBig = false;

    bool m_canJump = true;
    float m_startJumpPosition = 0.0f;

    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawCameraTrap = false;

    bool m_paused = false;
    bool m_gameOver = false;

    sf::Text m_gridText;
    sf::Text m_gameText;

    int m_levelTime = 60;

    void init();

    void sMovement();
    void sLifespan();
    void sCollision();
    void sAnimation();
    void sCamera();

    void loadLevel();
    void spawnPlayer(bool killCurrent = false);
    void spawnBullet(std::shared_ptr<Entity> entity);
    void spawnCoin(Vec2 position);
    void spawnMushroom(Vec2 position);

    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:
    SPlay(GameEngine* game, const std::string& levelPath);

    void update() override;
    void sRender() override;
    void sDoAction(const Action& action) override;
};

#endif //COMP4300_ASSIGNMENT3_SPLAY_H
