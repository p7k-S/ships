#include "Game.h"
#include "GameConfig.h"
#include <cmath>

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
    
    for (const auto& troop : playerTroops) {
        if (auto* t = static_cast<gl::Troop*>(troop.get())) {
            for (auto* cell : cellsInRange(*t->getCell(), hexMap, t->getView(), gl::RangeMode::VIEW)) {
                players[p_id]->addViewableCells(cell);
            }
        }
    }
    
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
}

void Game::nextTurn() {
    waitingForMove = false;
    selectedTroop = nullptr;
    selectedHex = nullptr;
    targetHex = nullptr;
    isProcessingTurn = true;
    troopsOnPortAction();
}
