#include "GameStateManager.h"

template<typename T>
void GameStateManager::registerState(const std::string& name) {
    states[name] = std::make_unique<T>();
}

void GameStateManager::changeState(const std::string& name) {
    auto it = states.find(name);
    if (it != states.end()) {
        if (currentState) {
            currentState->exit();
            previousState = currentState;
        }
        
        currentState = it->second.get();
        currentState->enter();
    }
}

void GameStateManager::returnToPreviousState() {
    if (previousState) {
        changeState(previousState->getStateName());
    }
}

void GameStateManager::update(float dt) {
    if (currentState) {
        currentState->update(dt);
    }
}

void GameStateManager::render(sf::RenderWindow& window) {
    if (currentState) {
        currentState->render(window);
    }
}

void GameStateManager::handleInput(const sf::Event& event) {
    if (currentState) {
        currentState->handleInput(event);
    }
}

GameState* GameStateManager::getCurrentState() const {
    return currentState;
}

std::string GameStateManager::getCurrentStateName() const {
    return currentState ? currentState->getStateName() : "None";
}

// Явные инстанцирования для всех состояний
template void GameStateManager::registerState<class MainMenuState>(const std::string&);
template void GameStateManager::registerState<class MyTurnState>(const std::string&);
template void GameStateManager::registerState<class WaitForTurnState>(const std::string&);
template void GameStateManager::registerState<class PauseState>(const std::string&);
template void GameStateManager::registerState<class GameOverState>(const std::string&);
