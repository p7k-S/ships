#include "MainMenuState.h"
#include "Game.h"

void MainMenuState::onEnter(Game& game) {
    initializeUI();
}

void MainMenuState::initializeUI() {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        // Fallback - создаем базовый шрифт
        font = sf::Font();
    }
    
    titleText.setFont(font);
    titleText.setString("Sea Battle Game");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(200, 100);
    
    startLocalText.setFont(font);
    startLocalText.setString("Local Game");
    startLocalText.setCharacterSize(32);
    startLocalText.setFillColor(sf::Color::Green);
    startLocalText.setPosition(300, 200);
    
    startNetworkText.setFont(font);
    startNetworkText.setString("Network Game");
    startNetworkText.setCharacterSize(32);
    startNetworkText.setFillColor(sf::Color::Yellow);
    startNetworkText.setPosition(300, 250);
    
    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(32);
    exitText.setFillColor(sf::Color::Red);
    exitText.setPosition(300, 300);
}

void MainMenuState::handleEvents(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                // Запуск локальной игры
                game.isNetworkGame = false;
                game.changeState(std::make_unique<MyTurnState>());
            }
            else if (event.key.code == sf::Keyboard::N) {
                // Запуск сетевой игры
                game.isNetworkGame = true;
                game.changeState(std::make_unique<WaitForTurnState>());
            }
            else if (event.key.code == sf::Keyboard::Escape) {
                game.window.close();
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = game.window.mapPixelToCoords(
                sf::Mouse::getPosition(game.window)
            );
            handleMenuSelection(game, mousePos);
        }
    }
}

void MainMenuState::handleMenuSelection(Game& game, const sf::Vector2f& mousePos) {
    if (startLocalText.getGlobalBounds().contains(mousePos)) {
        game.isNetworkGame = false;
        game.changeState(std::make_unique<MyTurnState>());
    }
    else if (startNetworkText.getGlobalBounds().contains(mousePos)) {
        game.isNetworkGame = true;
        game.changeState(std::make_unique<WaitForTurnState>());
    }
    else if (exitText.getGlobalBounds().contains(mousePos)) {
        game.window.close();
    }
}

void MainMenuState::update(Game& game) {
    // Анимации меню можно добавить здесь
}

void MainMenuState::render(Game& game) {
    game.window.clear(sf::Color(30, 30, 60));
    
    game.window.draw(titleText);
    game.window.draw(startLocalText);
    game.window.draw(startNetworkText);
    game.window.draw(exitText);
    
    game.window.display();
}
