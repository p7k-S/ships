#include "Game.h"
#include "GameLogic.h"
#include "../render/info_bars.h"
#include "../game/entity/Player.h"
// #include "../game/troops/Ship.h"
#include "../game/map/Cell.h"
// #include "../game/troops/BaseTroop.h"
// #include <cstdint>
#include <cstdint>
#include <iostream>

void Game::handleTroopSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            if (hex.hasTroop() && hex.getTroop()->isOwnedByCurrentPlayer(players[p_id].get())) {
                selectedTroop = hex.getTroop();
                waitingForMove = true;
                targetHex = nullptr;
                currentPath.clear();
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
            if (waitingForMove && selectedTroop && targetHex) {
                executeTroopAction();
                // updateVisibleCells();
                // render();
            }
            break;
        }
    }
}

void Game::executeTroopAction() {
    gl::Hex* selectedHex = selectedTroop->getCell();
    if (!selectedHex || selectedHex == targetHex || !getMoveAmount()) {
        resetSelection();
        return;
    }
    int8_t movesLeft = getMoveAmount();

    int fromQ = selectedHex->q;
    int fromR = selectedHex->r;
    int toQ = targetHex->q;
    int toR = targetHex->r;


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
        std::cout << "DEBUG: After move - troop cell: " 
            << (selectedTroop->getCell() ? "exists" : "nullptr") << std::endl;
        std::cout << "DEBUG: After move - troop in player list: " 
            << players[p_id]->getTroops().size() << std::endl;
        std::cout << "DEBUG: After move - troop on target cell: " 
            << (targetHex->getTroop() == selectedTroop) << std::endl;

        selectedTroop->takeGoldFromCell(targetHex);
        selectedTroop->takeItemByIndexFromCell(0);
        addViewedCells(players[p_id]->getSeenCells(), selectedTroop, hexMap, gl::RangeMode::VIEW);
        // updateVisibleCells();

    } else if (std::find(attackRangeHexes.begin(), attackRangeHexes.end(), targetHex) != attackRangeHexes.end()) {
            if (targetHex->hasTroop() && isEnemy(targetHex->getTroop()->getOwner(), selectedTroop->getOwner())) {
                selectedTroop->giveDamageToTroop(targetHex);
                // auto t = targetHex->getTroop();
                // std::cout << "TTTTTTTTTTTTTTT\n";
                // t->giveDamageToTroop(selectedHex);
            } else if (targetHex->hasBuilding() && isEnemy(targetHex->getBuilding()->getOwner(), selectedTroop->getOwner())) {
                selectedTroop->giveDamageToBuilding(targetHex);
            } else {
                ++movesLeft;
                std::cout << "target hex is not in any range.\n";
            }
    } else {
        ++movesLeft;
        std::cout << "target hex is not in any range.\n";
    }

    --movesLeft;
    setMoveAmount(movesLeft);
    std::cout << "Moves left "<< (int)movesLeft << "\n";

    resetSelection();
}


void Game::troopsOnPortAction() {
    for (const auto& troop : players[p_id]->getTroops()) {
        auto cell = troop->getCell();
        if (cell->hasBuilding()) {
            troop->addHP(cell->getBuilding()->getHeal());
            troop->takeGold(cell->getBuilding()->getGold());

            if (cell->getBuilding()->isPort() && troop->hasItem()) {
                gl::Port* port = static_cast<gl::Port*>(cell->getBuilding());
                port->addItem(troop->loseItem());
                players[p_id]->incItemsCount();
                
                if (players[p_id]->getItemsCount() == treasuresAmount) {
                    endGame = true;
                }
            }
        }
    }
}
