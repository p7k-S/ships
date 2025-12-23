#include "Game.h"
#include "../render/ui/startUI.h"
#include "../textures/EmbeddedResources.h"

bool Game::startMenu() {
    SimpleConfigUI configUI;
    bool result = configUI.show();
    return result;
}

bool Game::initialize() {
    uiView = window.getDefaultView();
    mapView = sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    view = mapView;
    defaultView = mapView;

    generateMap();
    distributeCellTypes();
    placeGoldAndTreasures();

    return initializeWindow() && loadTextures();
}

bool Game::initializeWindow() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "Pro game 228", sf::Style::Default, settings);
    view = window.getDefaultView();
    defaultView = view;
    return true;
}

bool Game::loadTextures() {
    if (!EmbeddedResources::loadAllResources()) return false;
    return true;
}
