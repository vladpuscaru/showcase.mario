//
// Created by Vlad Puscaru on 12.12.2023.
//

#include "Scene.h"

const std::map<sf::Keyboard::Key, Action::Actions>& Scene::getActionMap() const {
    return m_actionMap;
}

void Scene::doAction(const Action &action) {
    sDoAction(action);
}

void Scene::simulate(int times) {
    for (int i = 0; i < times; i++) {
        update();
    }
}

void Scene::registerAction(sf::Keyboard::Key key, Action::Actions actionName) {
    m_actionMap.insert(std::make_pair(key, actionName));
}

Scene::Scene(GameEngine* game)
    : m_game(game)
{

}
