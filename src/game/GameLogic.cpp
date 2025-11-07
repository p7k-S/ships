#include "Game.h"
#include "../render/info_bars.h"
#include <iostream>

void Game::handleShipSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            if (hex.getShip() && hex.getShip()->getOwner() == gl::Owner::PLAYER) {
                selectedShip = hex.getShip();
                waitingForMove = true;
                targetHex = nullptr;
                currentPath.clear();
                std::cout << "Корабль выбран.\n";
            } else {
                selectedHex = &hex;
                selectedShip = nullptr;
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
    if (!selectedShip || !targetHex) return;

    if (selectedShip->canMoveTo(targetHex, hexMap)) {
        selectedShip->moveTo(targetHex, hexMap);
        selectedShip->takeGoldFromCell(targetHex);
        addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);
    } else if (targetHex->getShip() && targetHex->getShip()->getOwner() != gl::Owner::FRIENDLY) {
        selectedShip->giveDamage(targetHex, hexMap);
        if (targetHex->getShip()->isDestroyed())
            std::cout << "Вражеский корабль уничтожен!\n";
    } else {
        std::cout << "Невозможно выполнить действие.\n";
    }

    resetSelection();
}

