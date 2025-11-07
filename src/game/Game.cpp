#include "Game.h"
#include <iostream>

Game::Game() {}

void Game::run() {
    if (!initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return;
    }

    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::update() {
    cleanupDestroyedShips();
    updateVisibleCells();
}
