#include "WaitForTurnState.h"
#include "Game.h"
#include "MyTurnState.h"
#include "WaitTroopSelectedState.h"

void WaitForTurnState::onEnter(Game& game) {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        font = sf::Font();
    }
    
    waitText.setFont(font);
    waitText.setString("Waiting for other player's turn...");
    waitText.setCharacterSize(24);
    waitText.setFillColor(sf::Color::Yellow);
}

void WaitForTurnState::handleEvents(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                game.changeState(std::make_unique<PauseState>());
            }
        }
        
        // В режиме ожидания разрешаем только просмотр
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f worldPos = game.window.mapPixelToCoords(
                    sf::Mouse::getPosition(game.window)
                );
                
                // Можно выбирать юнитов только для просмотра
                game.handleTroopSelection(worldPos);
                if (game.selectedTroop) {
                    game.changeState(std::make_unique<WaitTroopSelectedState>());
                }
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            game.handleMouseMove(event);
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            game.handleMouseWheel(event);
        }
    }
    
    // Проверяем сетевые сообщения
    if (game.isNetworkGame) {
        game.handleNetworkMessages();
    }
}

void WaitForTurnState::update(Game& game) {
    // Проверяем, не наш ли теперь ход
    if (game.p_id == game.my_pid) {
        game.changeState(std::make_unique<MyTurnState>());
    }
    
    game.updateVisibleCells();
}

void WaitForTurnState::render(Game& game) {
    game.renderWaitMove();
    
    // Оверлей ожидания
    sf::RectangleShape overlay(sf::Vector2f(game.window.getSize().x, game.window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    game.window.draw(overlay);
    
    waitText.setPosition(
        game.window.getSize().x / 2 - waitText.getGlobalBounds().width / 2,
        game.window.getSize().y / 2 - waitText.getGlobalBounds().height / 2
    );
    game.window.draw(waitText);
    
    game.window.display();
}
