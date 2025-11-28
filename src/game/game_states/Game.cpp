#include "Game.h"
#include "MainMenuState.h"
#include "MyTurnState.h"
#include "WaitForTurnState.h"
#include "PauseState.h"
#include "GameOverState.h"

Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (!instance) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() : window(sf::VideoMode(800, 600), "Turn-Based Strategy") {
    initialize();
}

void Game::initialize() {
    // Регистрация всех состояний
    stateManager.registerState<MainMenuState>("MainMenu");
    stateManager.registerState<MyTurnState>("MyTurn");
    stateManager.registerState<WaitForTurnState>("WaitForTurn");
    stateManager.registerState<PauseState>("Pause");
    stateManager.registerState<GameOverState>("GameOver");
    
    // Начальное состояние
    stateManager.changeState("MainMenu");
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        stateManager.handleInput(event);
    }
}

void Game::update(float dt) {
    stateManager.update(dt);
}

void Game::render() {
    window.clear();
    stateManager.render(window);
    window.display();
}

GameStateManager& Game::getStateManager() {
    return stateManager;
}

sf::RenderWindow& Game::getWindow() {
    return window;
}
