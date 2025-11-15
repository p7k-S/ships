#include "Game.h"
// #include <cstdint>
#include <iostream>

Game::Game() {}

void Game::run() {
    if (!initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return;
    }

    processPlayerTurn(); // начинает нулевой
    while (window.isOpen()) {
        processEvents();

        if (!isProcessingTurn) {
            p_id = (p_id + 1) % playersAmount;
            totalTurnCount++;
            isProcessingTurn = true;
            std::cout << "Player " << (int)p_id << " turn started!" << std::endl;
            processPlayerTurn();
        }

        render();

    }
}

void Game::processPlayerTurn() {
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
