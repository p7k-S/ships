#include "Game.h"

void Game::run() {
    window.setFramerateLimit(60);

    if (!startMenu()) return;

    if (players.empty()) {
        createPlayers();
    }

    if (!initialize()) return;

    createTroops();

    while (window.isOpen()) {
        processEvents();

        if (endGame) {
            if (!lose) {
                showVictoryScreen(p_id);
            } else {
                showLoseScreen();
            }
        } else { 
            if (isProcessingTurn) {
                totalTurnCount++;
                isProcessingTurn = false;
                p_id = nextAlivePlayer();
            }

            if (changeTurnLocal) {
                renderWaitMove();
            }
            else {
                if (Rerender)
                    render();
                Rerender = false;
            }
        }
        sf::sleep(sf::milliseconds(1));
    }
}

uint8_t Game::nextAlivePlayer() {
    if (playersAmount > 1) {
        for (uint8_t i = 1; i < playersAmount; ++i) {
            uint8_t nextPlayer = (p_id + i) % playersAmount;
            if (nextPlayer < (p_id + i)) {
                processBotsTrurns();
            }

            if (!players[nextPlayer]->isDead()) {
                setMoveAmount(players[nextPlayer]->getTroops().size());
                return nextPlayer;
            }
        }
        --totalTurnCount;
        endGame = true;
        return p_id;
    } else {
        if (totalTurnCount > 1)
            processBotsTrurns();

        setMoveAmount(players[0]->getTroops().size());
        if (players[0]->isDead()) {
            endGame = true;
            lose = true;
        }
        return 0;
    }
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
