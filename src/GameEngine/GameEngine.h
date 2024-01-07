//
// Created by Vlad Puscaru on 12.12.2023.
//

#ifndef COMP4300_ASSIGNMENT3_GAMEENGINE_H
#define COMP4300_ASSIGNMENT3_GAMEENGINE_H

#include "../Scenes/Scene/Scene.h"
#include "../Scenes/SPlay/SPlay.h"
#include "../Scenes/SMenu/SMenu.h"
#include "../Assets/Assets.h"
#include <map>
#include <SFML/Graphics.hpp>


typedef std::vector<std::vector<std::string>> FileContent;

class GameEngine {
private:
    std::map<Scene::SceneType, std::shared_ptr<Scene>> m_scenes;
    Scene::SceneType m_currentScene;
    Assets m_assets;
    sf::RenderWindow m_window;
    bool m_running;

    void loadAssetsFromFile();
public:
    GameEngine(int sWidth, int sHeight, const std::string& title);

    void init();

    std::shared_ptr<Scene> currentScene();

    bool isRunning() const;

    void run();
    void update();
    void quit();

    void changeScene(Scene::SceneType sceneType, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
    const Assets& getAssets() const;
    sf::RenderWindow& getWindow();

    void sUserInput();

    std::vector<std::string> splitString(const std::string &str, char del) {
        std::vector<std::string> words;
        int last = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == del) {
                words.push_back(str.substr(last, i - last));
                last = i + 1;
            }
        }
        words.push_back(str.substr(last, str.length() - last));
        return words;
    }

    FileContent splitFileContent(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file " << filename << std::endl;
            exit(-1);
        }

        FileContent content;

        std::string line;
        while (getline(file, line)) {
            std::vector<std::string> tokens = splitString(line, ' ');
            content.emplace_back(tokens);
        }

        return content;
    }
};


#endif //COMP4300_ASSIGNMENT3_GAMEENGINE_H
