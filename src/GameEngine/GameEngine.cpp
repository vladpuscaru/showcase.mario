//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "GameEngine.h"

#include <utility>

GameEngine::GameEngine(int sWidth, int sHeight, const std::string &title)
    : m_window(sf::VideoMode(sWidth, sHeight), title)
{
    init();
}

void GameEngine::init() {
    m_window.setFramerateLimit(60);
    m_running = true;

    loadAssetsFromFile();

    std::shared_ptr<Scene> scene = std::make_shared<SMenu>(this);
    changeScene(Scene::MENU, scene);
}

std::shared_ptr<Scene> GameEngine::currentScene() {
    return m_scenes[m_currentScene];
}

void GameEngine::run() {
    while (isRunning()) {
        update();
    }
}

void GameEngine::update() {
    sUserInput();
    currentScene()->update();
    currentScene()->sRender();
}

void GameEngine::quit() {
    m_running = false;
    m_window.close();
}

void GameEngine::changeScene(Scene::SceneType sceneType, std::shared_ptr<Scene> scene, bool endCurrentScene) {
    if (endCurrentScene) {
        m_scenes.erase(m_currentScene);
    }

    if (m_scenes.find(sceneType) == m_scenes.end()) {
        m_scenes.insert(std::make_pair(sceneType, scene));
    }

    m_currentScene = sceneType;
}

const Assets &GameEngine::getAssets() const {
    return m_assets;
}

sf::RenderWindow &GameEngine::getWindow() {
    return m_window;
}

void GameEngine::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quit();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::X) {
                // TODO: dynamically create unique file name (based on time?)
                sf::Texture texture;
                texture.create(m_window.getSize().x, m_window.getSize().y);
                texture.update(m_window);
                if (texture.copyToImage().saveToFile("test.png")) {
                    std::cout << "Screenshot saved to " << "test.png" << std::endl;
                } else {
                    std::cerr << "Error trying to save screenshot to " << "test.png" << std::endl;
                }
            }
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            // Skip if current scene does not have an action associated with this key
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
                continue;
            }

            Action::ActionTypes actionType = (event.type == sf::Event::KeyPressed) ? Action::START : Action::STOP;
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }
    }
}

void GameEngine::loadAssetsFromFile() {
    FileContent assetsConfig = splitFileContent("./resources/config/assets.txt");

    for (auto& line : assetsConfig) {
        if (line[0] == "Font") {
            std::string& fontName = line[1];
            std::string& fontFilePath = line[2];
            m_assets.addFont(fontName, fontFilePath);
        }

        if (line[0] == "Texture") {
            std::string& textureName = line[1];
            std::string& textureFilePath = line[2];
            m_assets.addTexture(textureName, textureFilePath);
        }

        if (line[0] == "Animation") {
            std::string& animationName = line[1];
            std::string& animationTexture = line[2];
            int animationFrames = std::atoi(line[3].c_str());
            int animationSpeed = std::atoi(line[4].c_str());
            bool loop = line[5] == "loop";

            std::shared_ptr<Animation> animation = std::make_shared<Animation>(animationName, m_assets.getTexture(animationTexture), animationFrames, animationSpeed, loop);
            m_assets.addAnimation(animationName, animation);
        }
    }
}

bool GameEngine::isRunning() const {
    return m_running && m_window.isOpen();
}
