#include "Game.h"
// #include <iostream>
#include "../render/ui/startUI.h"
// #include "../render/Colors.hpp"

bool Game::startMenu() {
    SimpleConfigUI configUI;
    if (configUI.show()) {
        // настройка перед стартом, если нужно
    }
    return true;
}

bool Game::initialize() {
    if (!startMenu()) return false;

    if (!font.loadFromFile(font_path)) {
        std::cout << "Font not found, using default" << std::endl;
    }

    defaultView = window.getDefaultView();
    mapView = defaultView;  // стартовая позиция карты такая же, как default

    generateMap();
    distributeCellTypes();
    createPlayers();
    createShips();
    placeGoldAndTreasures();

    return initializeWindow() && loadTextures();
}

bool Game::initializeWindow() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default, settings);
    view = window.getDefaultView();
    defaultView = view;
    return true;
}

bool Game::loadTextures() {
    if (!player_ship_texture.loadFromFile("../src/textures/player_ship.png")) return false;
    if (!pirate_ship_texture.loadFromFile("../src/textures/pirates_ship.png")) return false;
    if (!enemy_ship_texture.loadFromFile("../src/textures/enemy_ship.png")) return false;
    if (!gold_texture.loadFromFile("../src/textures/gold_cons.png")) return false;
    if (!treasure_texture.loadFromFile("../src/textures/treasure.png")) return false;

    return true;
}

