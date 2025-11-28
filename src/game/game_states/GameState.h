#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameState {
public:
    virtual ~GameState() = default;
    
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleInput(const sf::Event& event) = 0;
    
    virtual std::string getStateName() const = 0;
};
