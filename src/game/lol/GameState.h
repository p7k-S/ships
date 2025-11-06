#pragma once

#include "GameLogic.h"
#include "map.h"
#include "ship.h"
#include <vector>
#include <memory>
#include <random>

namespace gl = GameLogic;
// #include "entities/Ship.h"
// #include "map/Hex.h"

class GameState {
private:
    std::vector<gl::Hex> hexMap;
    std::vector<std::unique_ptr<gl::Ship>> ships;
    std::vector<gl::Hex*> seenCells;
    std::vector<gl::Hex*> vieweableHexes;
    
    gl::Ship* selectedShip = nullptr;
    gl::Hex* selectedHex = nullptr;
    gl::Hex* targetHex = nullptr;
    
    bool waitingForMove = false;
    int turnCount = 1;
    
public:
    // Getters
    const std::vector<gl::Hex>& getHexMap() const { return hexMap; }
    const std::vector<std::unique_ptr<gl::Ship>>& getShips() const { return ships; }
    gl::Ship* getSelectedShip() const { return selectedShip; }
    
    // Game logic
    void handleInput(const sf::Event& event);
    void updateGameLogic();
    void cleanupDestroyedShips();
    void updateVisibility();
    
    // Selection
    void selectShip(gl::Ship* ship);
    void selectTarget(gl::Hex* hex);
    void confirmAction();
    
    // Pathfinding
    std::vector<gl::Hex*> calculatePath(gl::Hex* start, gl::Hex* end);
};
