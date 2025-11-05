#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"

class InputHandler {
public:
    void setWindow(sf::RenderWindow* window) { this->window = window; }
    void handleEvents(Game& game);

private:
    void handleKeyPress(const sf::Event& event, Game& game);
    void handleMouseClick(const sf::Event& event, Game& game);
    void handleMouseWheel(const sf::Event& event, Game& game);
    
    sf::RenderWindow* window = nullptr;
};
