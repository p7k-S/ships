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

bool Game::isPlayerOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Player*>(owner);
}

bool Game::isEnemyOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Enemy*>(owner);
}

bool Game::isPirateOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Pirate*>(owner);
}

