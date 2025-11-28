#pragma once
#include "GameState.h"
#include <unordered_map>
#include <memory>
#include <string>

class GameStateManager {
private:
    std::unordered_map<std::string, std::unique_ptr<GameState>> states;
    GameState* currentState = nullptr;
    GameState* previousState = nullptr;

public:
    template<typename T>
    void registerState(const std::string& name);
    
    void changeState(const std::string& name);
    void returnToPreviousState();
    
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleInput(const sf::Event& event);
    
    GameState* getCurrentState() const;
    std::string getCurrentStateName() const;
};
