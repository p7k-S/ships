#include "Game.h"
// #include <iostream>
#include "../render/ui/startUI.h"
#include "../textures/EmbeddedResources.h"
// #include "../render/Colors.hpp"

bool Game::startMenu() {
    std::cout << "DEBUG: startMenu() started" << std::endl;
    SimpleConfigUI configUI;
    bool result = configUI.show();
    std::cout << "DEBUG: startMenu() finished, returning: " << (result ? "true" : "false") << std::endl;
    return result;
}

bool Game::initialize() {
    // if (!font.loadFromFile(font_path)) {
    //     std::cout << "Font not found, using default" << std::endl;
    // }

    defaultView = window.getDefaultView();
    mapView = defaultView;  // стартовая позиция карты такая же, как default

    generateMap();
    distributeCellTypes();
    // createPlayers();
    // createShips();
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
    if (!EmbeddedResources::loadAllResources()) return false;
    return true;
}

