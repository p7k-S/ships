#include "input_manager.h"
// #include "HexMap.h"
#include "ShipManager.h"

void InputHandler::handleEvents(Game& game) {
    sf::Event event;
    while (window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::KeyPressed:
                handleKeyPress(event, game);
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(event, game);
                }
                break;
            case sf::Event::MouseWheelScrolled:
                handleMouseWheel(event, game);
                break;
        }
    }
}

void InputHandler::handleKeyPress(const sf::Event& event, Game& game) {
    if (event.key.code == sf::Keyboard::R) {
        game.getRenderer().toggleColorScheme();
    }
    // Обработка перемещения камеры...
}

void InputHandler::handleMouseClick(const sf::Event& event, Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    game.getShipManager().handleMouseClick(mousePos, game.getHexMap());
}
