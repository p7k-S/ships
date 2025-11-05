#include "Game.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "../game/troops/Ship.h"
#include <iostream>
// #include <algorithm> // Добавлено для std::remove_if

// Уберите эту строку, так как она уже есть в Game.h
// namespace gl = GameLogic;

Game::Game(std::vector<gl::Hex>& hexMap, 
           std::vector<std::unique_ptr<gl::Ship>>& ships,
           std::vector<gl::Hex*>& seenCells)
    : hexMap(hexMap), ships(ships), seenCells(seenCells) {
}

bool Game::initialize() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default, settings);
    
    view = window.getDefaultView();
    defaultView = view;
    
    return loadTextures(); // ИЗМЕНЕНО: убрана проверка if
}

bool Game::loadTextures() {
    bool success = true;
    
    if (!player_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/player_ship.png")) {
        std::cout << "Failed to load player_ship.png" << std::endl;
        success = false;
    }
    if (!pirate_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/pirates_ship.png")) {
        std::cout << "Failed to load pirates_ship.png" << std::endl;
        success = false;
    }
    if (!enemy_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/enemy_ship.png")) {
        std::cout << "Failed to load enemy_ship.png" << std::endl;
        success = false;
    }
    if (!gold_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/gold_cons.png")) {
        std::cout << "Failed to load gold_cons.png" << std::endl;
        success = false;
    }
    if (!treasure_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/treasure.png")) {
        std::cout << "Failed to load treasure.png" << std::endl;
        success = false;
    }
    
    return success;
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        InputHandler::handleGameInput(event, window, view, defaultView, 
                                    selectedShip, waitingForMove, targetHex, 
                                    currentPath, isDragging, lastMousePos,
                                    hexMap, ships);
    }
}

void Game::update() {
    cleanupDestroyedShips();
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    Renderer::renderGame(window, view, hexMap, ships, seenCells,
                        selectedHex, selectedShip, waitingForMove, targetHex,
                        currentPath, player_ship_texture, pirate_ship_texture,
                        enemy_ship_texture, gold_texture, treasure_texture);
    
    window.display();
}

void Game::cleanupDestroyedShips() {
    // ИЗМЕНЕНО: безопасное удаление без вызова Destroy()
    auto it = ships.begin();
    while (it != ships.end()) {
        if ((*it)->isDestroyed()) {
            it = ships.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::cleanup() {
    // Очистка ресурсов если нужно
}
