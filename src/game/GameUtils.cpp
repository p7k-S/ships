#include "Game.h"
#include "GameConfig.h"
#include <cmath>
#include <unordered_set>
// #include <memory> // для std::shared_ptr


bool Game::isPointInHex(const sf::Vector2f& point, const gl::Hex& hex) {
    float hx = hex.q * hexRadius * 1.5 + 50;
    float hy = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0 + 50;
    float dx = point.x - hx;
    float dy = point.y - hy;
    return std::sqrt(dx * dx + dy * dy) <= hexRadius;
}

void Game::cleanupDestroyedShips() {
    // for (auto& ship : ships)
    //     if (ship->isDestroyed())
    //         ship->Destroy();
    //
    // ships.erase(
    //     std::remove_if(ships.begin(), ships.end(),
    //         [](const std::shared_ptr<gl::Ship>& s) { return s->isDestroyed(); }),
    //     ships.end());
}

void Game::updateVisibleCells() {
    viewableHexes.clear();
    std::unordered_set<gl::Hex*> unique;
    for (auto& player : players) {
        const auto& playerTroops = player->getTroops();
        for (const auto& troop : playerTroops) {
            if (auto* ship = static_cast<gl::Ship*>(troop.get())) {
                for (auto* cell : cellsInRange(*ship->getCell(), hexMap, ship->getView(), gl::RangeMode::VIEW)) {
                    if (unique.insert(cell).second) {
                        viewableHexes.push_back(cell);
                    }
                }
            }
        }
    }
}

void Game::resetSelection() {
    waitingForMove = false;
    selectedTroop = nullptr;
    selectedHex = nullptr;
    targetHex = nullptr;
    currentPath.clear();
}

void Game::cleanup() {
    // ships.clear();
    seenCells.clear();
    viewableHexes.clear();
    currentPath.clear();
}
