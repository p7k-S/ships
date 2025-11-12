#pragma once
#include <SFML/Graphics.hpp>

class Game; // forward declaration

class GameState {
public:
    virtual ~GameState() = default;
    
    virtual void handleEvents(Game& game) = 0;
    virtual void update(Game& game, float deltaTime) = 0;
    virtual void render(Game& game) = 0;
    virtual void onEnter(Game& game) {}
    virtual void onExit(Game& game) {}
};
