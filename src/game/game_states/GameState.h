#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class Game;

class GameState {
public:
    virtual ~GameState() = default;
    
    virtual void handleEvents(Game& game) = 0;
    virtual void update(Game& game) = 0;
    virtual void render(Game& game) = 0;
    virtual void onEnter(Game& game) {}
    virtual void onExit(Game& game) {}
    
    virtual std::string getName() const = 0;
};
