#include "Game.h"
#include "GameLogic.h"
#include "../render/info_bars.h"
#include "../game/entity/Player.h"
#include "../game/troops/Ship.h"
#include "../game/map/Cell.h"
// #include "../game/troops/BaseTroop.h"
#include <iostream>

void Game::handleShipSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            if (hex.getTroopOf<gl::Ship>() && std::holds_alternative<gl::Player*>(hex.getTroopOf<gl::Ship>()->getOwner())) {
                selectedShip = hex.getTroopOf<gl::Ship>();
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
    } else if (targetHex->hasTroopOf<gl::Ship>() && isEnemy(targetHex->getTroopOf<gl::Ship>()->getOwner(), players[0])) {
        selectedShip->giveDamage(targetHex, hexMap);
        if (targetHex->getTroopOf<gl::Ship>()->isDestroyed())
            std::cout << "Вражеский корабль уничтожен!\n";
    } else {
        std::cout << "Невозможно выполнить действие.\n";
    }

    resetSelection();
}

