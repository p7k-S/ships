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

bool Game::isPlayerOwner(const gl::Owner& owner) const {
    return std::holds_alternative<gl::Player*>(owner) && 
           std::get<gl::Player*>(owner) == getPlayer();
}

bool Game::isEnemyOwner(const gl::Owner& owner) const {
    return std::holds_alternative<gl::Enemy*>(owner) && 
           std::get<gl::Enemy*>(owner) == getEnemy();
}

bool Game::isPirateOwner(const gl::Owner& owner) const {
    return std::holds_alternative<gl::Pirate*>(owner) && 
           std::get<gl::Pirate*>(owner) == getPirate();
}
