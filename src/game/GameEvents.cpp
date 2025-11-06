#include "Game.h"
#include <SFML/Window/Event.hpp>

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed: window.close(); break;
            case sf::Event::KeyPressed: handleKeyPressed(event); break;
            case sf::Event::MouseButtonPressed: handleMouseButtonPressed(event); break;
            case sf::Event::MouseWheelScrolled: handleMouseWheel(event); break;
            case sf::Event::MouseMoved: handleMouseMove(event); break;
            default: break;
        }
    }
}

