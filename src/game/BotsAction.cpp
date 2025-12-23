#include "Game.h"
#include <climits>

void Game::processBotsTrurns() {
    execPirateAction();
    execEnemyAction();
}


void Game::execPirateAction() {
    const auto& pirates = pirate.getTroops();
    
    for (auto& pirateTroop : pirates) {
        if (!pirateTroop) continue;
        
        gl::Hex* currentHex = pirateTroop->getCell();
        if (!currentHex) continue;
        
        std::vector<gl::Hex*> visibleHexes = cellsInRange(
            *currentHex, hexMap, pirateTroop->getView(), gl::RangeMode::VIEW);
        
        gl::Hex* closestEnemy = nullptr;
        int minDistance = pirateTroop->getView() + 1;
        
        for (gl::Hex* hex : visibleHexes) {
            if (hex->hasTroop()) {
                gl::Troop* troop = hex->getTroop();
                
                if (typeid(*troop) == typeid(gl::Ship) && 
                    isEnemy(troop->getOwner(), gl::Owner(&pirate))) {
                    
                    int distance = hexDistance(*currentHex, *hex);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestEnemy = hex;
                    }
                }
            }
        }
        
        if (closestEnemy != nullptr) {
            std::vector<gl::Hex*> attackRangeHexes = cellsInRange(
                *currentHex, hexMap, pirateTroop->getDamageRange(), gl::RangeMode::DAMAGE);
            
            if (std::find(attackRangeHexes.begin(), attackRangeHexes.end(), closestEnemy) 
                != attackRangeHexes.end()) {
                
                gl::Troop* enemyTroop = closestEnemy->getTroop();
                if (enemyTroop) {
                    pirateTroop->giveDamageToTroop(closestEnemy);
                    continue;
                }
            }
            
            std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                *currentHex, hexMap, pirateTroop->getMoveRange(), gl::RangeMode::MOVE);
            
            gl::Hex* bestMoveHex = nullptr;
            int bestDistance = pirateTroop->getView() + 1;
            
            for (gl::Hex* moveHex : moveRangeHexes) {
                if (!moveHex->hasTroop() && !moveHex->hasBuilding() && 
                    pirateTroop->canMoveTo(moveHex)) {
                    int distanceToEnemy = hexDistance(*moveHex, *closestEnemy);
                    if (distanceToEnemy < bestDistance) {
                        bestDistance = distanceToEnemy;
                        bestMoveHex = moveHex;
                    }
                }
            }
            
            if (bestMoveHex != nullptr) {
                pirateTroop->moveTo(bestMoveHex);
                pirateTroop->takeGoldFromCell(bestMoveHex);
                continue;
            }
        }
        
        std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
            *currentHex, hexMap, pirateTroop->getMoveRange(), gl::RangeMode::MOVE);
        
        std::vector<std::pair<gl::Hex*, int>> availableMovesWithGold;
        
        for (gl::Hex* moveHex : moveRangeHexes) {
            if (!moveHex->hasTroop() && !moveHex->hasBuilding() && 
                pirateTroop->canMoveTo(moveHex)) {
                
                int goldAmount = moveHex->getGold();
                availableMovesWithGold.emplace_back(moveHex, goldAmount);
            }
        }
        
        if (!availableMovesWithGold.empty()) {
            std::sort(availableMovesWithGold.begin(), availableMovesWithGold.end(),
                [](const std::pair<gl::Hex*, int>& a, const std::pair<gl::Hex*, int>& b) {
                    return a.second > b.second;
                });
            
            gl::Hex* bestGldHex = availableMovesWithGold[0].first;
            
            std::vector<gl::Hex*> bestGoldCells;
            int maxGold = availableMovesWithGold[0].second;
            
            for (const auto& [hex, gold] : availableMovesWithGold) {
                if (gold == maxGold) {
                    bestGoldCells.push_back(hex);
                } else {
                    break;
                }
            }
            
            int randomIndex = bestGoldCells.size() > 1 ? rand() % bestGoldCells.size() : 0;
            gl::Hex* targetHex = bestGoldCells[randomIndex];
            
            pirateTroop->moveTo(targetHex);
            pirateTroop->takeGoldFromCell(targetHex);
        } else {
            std::vector<gl::Hex*> availableMoves;
            for (gl::Hex* moveHex : moveRangeHexes) {
                if (!moveHex->hasTroop() && !moveHex->hasBuilding() && 
                    pirateTroop->canMoveTo(moveHex)) {
                    availableMoves.push_back(moveHex);
                }
            }
            
            if (!availableMoves.empty()) {
                int randomIndex = rand() % availableMoves.size();
                gl::Hex* randomHex = availableMoves[randomIndex];
                
                pirateTroop->moveTo(randomHex);
                pirateTroop->takeGoldFromCell(randomHex);
            }
        }
    }
}

void Game::execEnemyAction() {
    const auto& enemies = enemy.getTroops();
    
    for (auto& enemyTroop : enemies) {
        if (!enemyTroop) continue;
        
        gl::Hex* currentHex = enemyTroop->getCell();
        if (!currentHex) continue;
        
        bool isShip = (typeid(*enemyTroop) == typeid(gl::Ship));
        
        std::vector<gl::Hex*> visibleHexes = cellsInRange(
            *currentHex, hexMap, enemyTroop->getView(), gl::RangeMode::VIEW);
        
        if (!enemyTroop->hasItem()) {
            gl::Hex* closestItemHex = nullptr;
            int minItemDistance = enemyTroop->getView() + 1;
            
            for (gl::Hex* hex : visibleHexes) {
                if (hex->getItemsSize() > 0) {
                    int distance = hexDistance(*currentHex, *hex);
                    if (distance < minItemDistance) {
                        minItemDistance = distance;
                        closestItemHex = hex;
                    }
                }
            }
            
            if (closestItemHex != nullptr) {
                if (hexDistance(*currentHex, *closestItemHex) <= 1) {
                    enemyTroop->takeItemByIndexFromCell(0);
                    continue;
                }
                
                std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                    *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
                
                gl::Hex* bestMoveHex = nullptr;
                int bestDistance = enemyTroop->getView() + 1;
                
                for (gl::Hex* moveHex : moveRangeHexes) {
                    if (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex)) {
                        int distanceToItem = hexDistance(*moveHex, *closestItemHex);
                        if (distanceToItem < bestDistance) {
                            bestDistance = distanceToItem;
                            bestMoveHex = moveHex;
                        }
                    }
                }
                
                if (bestMoveHex != nullptr) {
                    enemyTroop->moveTo(bestMoveHex);
                    continue;
                }
            }
        }
        
        if (enemyTroop->hasItem()) {
            const auto& buildings = enemy.getBuildings();
            gl::Hex* closestPort = nullptr;
            int minPortDistance = INT_MAX;
            
            for (const auto& building : buildings) {
                if (!building) continue;
                
                gl::Hex* portHex = building->getCell();
                if (!portHex) continue;
                
                int distance = hexDistance(*currentHex, *portHex);
                if (distance < minPortDistance) {
                    minPortDistance = distance;
                    closestPort = portHex;
                }
            }
            
            if (closestPort != nullptr) {
                if (currentHex == closestPort) {
                    gl::Port* port = static_cast<gl::Port*>(closestPort->getBuilding());
                    port->addItem(enemyTroop->loseItem());

                    if (port->getItemsSize() == treasuresAmount) {
                        lose = true;
                        endGame = true;
                    }

                    continue;
                }
                
                std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                    *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
                
                gl::Hex* bestMoveHex = nullptr;
                int bestDistance = minPortDistance;
                
                for (gl::Hex* moveHex : moveRangeHexes) {
                    if (moveHex == closestPort || (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex))) {
                        int distanceToPort = hexDistance(*moveHex, *closestPort);
                        if (distanceToPort < bestDistance) {
                            bestDistance = distanceToPort;
                            bestMoveHex = moveHex;
                        }
                    }
                }
                
                if (bestMoveHex != nullptr) {
                    enemyTroop->moveTo(bestMoveHex);
                    continue;
                }
            }
        }
        
        gl::Hex* closestEnemyTroopHex = nullptr;
        int minEnemyDistance = enemyTroop->getView() + 1;
        
        for (gl::Hex* hex : visibleHexes) {
            if (hex->hasTroop()) {
                gl::Troop* troop = hex->getTroop();
                
                if (isEnemy(troop->getOwner(), gl::Owner(&enemy))) {
                    bool troopIsShip = (typeid(*troop) == typeid(gl::Ship));
                    
                    if ((isShip && troopIsShip) || (!isShip && !troopIsShip)) {
                        int distance = hexDistance(*currentHex, *hex);
                        if (distance < minEnemyDistance) {
                            minEnemyDistance = distance;
                            closestEnemyTroopHex = hex;
                        }
                    }
                }
            }
        }
        
        if (closestEnemyTroopHex != nullptr) {
            std::vector<gl::Hex*> attackRangeHexes = cellsInRange(
                *currentHex, hexMap, enemyTroop->getDamageRange(), gl::RangeMode::DAMAGE);
            
            if (std::find(attackRangeHexes.begin(), attackRangeHexes.end(), closestEnemyTroopHex) 
                != attackRangeHexes.end()) {
                
                gl::Troop* enemyTroopTarget = closestEnemyTroopHex->getTroop();
                if (enemyTroopTarget) {
                    enemyTroop->giveDamageToTroop(closestEnemyTroopHex);
                    continue;
                }
            }
            
            std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
            
            gl::Hex* bestMoveHex = nullptr;
            int bestDistance = enemyTroop->getView() + 1;
            
            for (gl::Hex* moveHex : moveRangeHexes) {
                if (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex)) {
                    int distanceToEnemy = hexDistance(*moveHex, *closestEnemyTroopHex);
                    if (distanceToEnemy < bestDistance) {
                        bestDistance = distanceToEnemy;
                        bestMoveHex = moveHex;
                    }
                }
            }
            
            if (bestMoveHex != nullptr) {
                enemyTroop->moveTo(bestMoveHex);
                continue;
            }
        }
        
        std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
            *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
        
        std::vector<std::pair<gl::Hex*, int>> availableMovesWithGold;
        
        for (gl::Hex* moveHex : moveRangeHexes) {
            if (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex)) {
                int goldAmount = 0;
                availableMovesWithGold.emplace_back(moveHex, goldAmount);
            }
        }
        
        if (!availableMovesWithGold.empty()) {
            std::sort(availableMovesWithGold.begin(), availableMovesWithGold.end(),
                [](const std::pair<gl::Hex*, int>& a, const std::pair<gl::Hex*, int>& b) {
                    return a.second > b.second;
                });
            
            std::vector<gl::Hex*> bestGoldCells;
            int maxGold = availableMovesWithGold[0].second;
            
            for (const auto& [hex, gold] : availableMovesWithGold) {
                if (gold == maxGold) {
                    bestGoldCells.push_back(hex);
                } else {
                    break;
                }
            }
            
            int randomIndex = bestGoldCells.size() > 1 ? rand() % bestGoldCells.size() : 0;
            gl::Hex* targetHex = bestGoldCells[randomIndex];
            
            enemyTroop->moveTo(targetHex);
            enemyTroop->takeGoldFromCell(targetHex);
            continue;
        }
        
        std::vector<gl::Hex*> availableMoves;
        for (gl::Hex* moveHex : moveRangeHexes) {
            if (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex)) {
                availableMoves.push_back(moveHex);
            }
        }
        
        if (!availableMoves.empty()) {
            int randomIndex = rand() % availableMoves.size();
            gl::Hex* randomHex = availableMoves[randomIndex];
            enemyTroop->moveTo(randomHex);
        }
    }
}
