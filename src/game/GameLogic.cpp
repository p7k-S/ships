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

    gl::Hex* selectedHex = selectedTroop->getCell();
    if (!selectedHex) return;

    // Диапазон перемещения
    std::vector<gl::Hex*> reachableHexes = cellsInRange(
            *selectedHex, hexMap, selectedTroop->getMoveRange(), gl::RangeMode::MOVE);
    for (gl::Hex* reachableHex : reachableHexes) {
        renderRangeHex(reachableHex, sf::Color(100, 255, 100, 80), sf::Color(112, 129, 88, 255));
    }

    // Диапазон атаки
    std::vector<gl::Hex*> attackRangeHexes = cellsInRange(
            *selectedHex, hexMap, selectedTroop->getDamageRange(), gl::RangeMode::DAMAGE);
    for (gl::Hex* attackHex : attackRangeHexes) {
        renderRangeHex(attackHex, sf::Color(200, 40, 40, 50), sf::Color::Transparent);
    }

    if (selectedTroop->canMoveTo(targetHex) && std::find(reachableHexes.begin(), reachableHexes.end(), targetHex)!= reachableHexes.end()) {
        selectedTroop->moveTo(targetHex);
        selectedTroop->takeGoldFromCell(targetHex);
        addViewedCells(seenCells, selectedTroop, hexMap, gl::RangeMode::VIEW);
    } else if (targetHex->hasTroop() && isEnemy(targetHex->getTroop()->getOwner(), selectedTroop->getOwner())
            && std::find(attackRangeHexes.begin(), attackRangeHexes.end(), targetHex)!= attackRangeHexes.end()) {
        selectedTroop->giveDamage(targetHex);
    } else {
        std::cout << "Невозможно выполнить действие.\n";
    }

    resetSelection();
}

