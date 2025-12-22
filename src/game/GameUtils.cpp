#include "Game.h"
#include "GameConfig.h"
#include <cmath>
// #include <cstdint>
// #include <memory> // для std::shared_ptr


bool Game::isPointInHex(const sf::Vector2f& point, const gl::Hex& hex) {
    float hx = hex.q * hexRadius * 1.5 + 50;
    float hy = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0 + 50;
    float dx = point.x - hx;
    float dy = point.y - hy;
    return std::sqrt(dx * dx + dy * dy) <= hexRadius;
}

void Game::updateVisibleCells() {
    players[p_id]->clearViewableCells();
    auto player = players[p_id].get();
    const auto& playerTroops = player->getTroops();
    
    // Troops VIEW
    for (const auto& troop : playerTroops) {
        if (auto* t = static_cast<gl::Troop*>(troop.get())) {
            for (auto* cell : cellsInRange(*t->getCell(), hexMap, t->getView(), gl::RangeMode::VIEW)) {
                players[p_id]->addViewableCells(cell);
            }
        }
    }
    
    // Buildings VIEW
    const auto& playerBuildings = player->getBuildings();
    for (const auto& build : playerBuildings) {
        if (auto* b = static_cast<gl::Building*>(build.get())) {
            for (auto* cell : cellsInRange(*b->getCell(), hexMap, b->getView(), gl::RangeMode::VIEW)) {
                players[p_id]->addViewableCells(cell);
            }
        }
    }
}

void Game::resetSelection() {
    waitingForMove = false;
    selectedTroop = nullptr;
    selectedHex = nullptr;
    targetHex = nullptr;
    // currentPath.clear();
}

void Game::nextTurn() {
    waitingForMove = false;
    selectedTroop = nullptr;
    selectedHex = nullptr;
    targetHex = nullptr;
    isProcessingTurn = true;
    // currentPath.clear();
    troopsOnPortAction();
}

// void Game::cleanup() {
//     if (!(players[p_id]->getTroops().size() || players[p_id]->getBuildings().size())) {
//         losers.push_back(std::move(players[p_id]));
//     }
//     // ships.clear();
//     // seenCells.clear();
//     // viewableHexes.clear();
//     // currentPath.clear();
// }
