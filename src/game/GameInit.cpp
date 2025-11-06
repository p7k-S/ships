#include "Game.h"

bool Game::initialize() {
    if (!startMenu()) return false;
    generateMap();
    distributeCellTypes();
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
    return player_ship_texture.loadFromFile("textures/player_ship.png")
        && pirate_ship_texture.loadFromFile("textures/pirates_ship.png")
        && enemy_ship_texture.loadFromFile("textures/enemy_ship.png")
        && gold_texture.loadFromFile("textures/gold_cons.png")
        && treasure_texture.loadFromFile("textures/treasure.png");
}

