#include "Game.h"
#include "GameLogic.h"
#include "../render/info_bars.h"
#include "../game/entity/Player.h"
// #include "../game/troops/Ship.h"
#include "../game/map/Cell.h"
// #include "../game/troops/BaseTroop.h"
#include <iostream>

void Game::handleShipSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            if (hex.hasTroop() && std::holds_alternative<gl::Player*>(hex.getTroop()->getOwner())) {
                selectedTroop = hex.getTroop();
                waitingForMove = true;
                targetHex = nullptr;
                currentPath.clear();
                std::cout << "Корабль выбран.\n";
            } else {
                selectedHex = &hex;
                selectedTroop = nullptr;
                waitingForMove = false;
            }
            break;
        }
    }
}

void Game::handleTargetSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            targetHex = &hex;
            std::cout << "Цель выбрана.\n";
            break;
        }
    }
}

void Game::executeShipAction() {
    if (!selectedTroop || !targetHex) return;

    if (selectedTroop->canMoveTo(targetHex)) {
        selectedTroop->moveTo(targetHex);
        selectedTroop->takeGoldFromCell(targetHex);
        addViewedCells(seenCells, selectedTroop, hexMap, gl::RangeMode::VIEW);
    } else if (targetHex->hasTroop() && isEnemy(targetHex->getTroop()->getOwner(), selectedTroop->getOwner())) {
        selectedTroop->giveDamage(targetHex);
        if (targetHex->getTroop()->isDestroyed())
            std::cout << "Вражеский корабль уничтожен!\n";
    } else {
        std::cout << "Невозможно выполнить действие.\n";
    }

    resetSelection();
}

