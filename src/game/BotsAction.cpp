#include "Game.h"
#include <climits>

void Game::processBotsTrurns() {
    execPirateAction();
    execEnemyAction();
}

// золото и файт, порты не трогают

void Game::execPirateAction() {
    const auto& pirates = pirate.getTroops();
    
    for (auto& pirateTroop : pirates) {
        if (!pirateTroop) continue;
        
        gl::Hex* currentHex = pirateTroop->getCell();
        if (!currentHex) continue;
        
        // 1. Получаем клетки видимости вокруг пирата
        std::vector<gl::Hex*> visibleHexes = cellsInRange(
            *currentHex, hexMap, pirateTroop->getView(), gl::RangeMode::VIEW);
        
        // 2. Ищем вражеские корабли в радиусе видимости
        gl::Hex* closestEnemy = nullptr;
        int minDistance = pirateTroop->getView() + 1;
        
        for (gl::Hex* hex : visibleHexes) {
            if (hex->hasTroop()) {
                gl::Troop* troop = hex->getTroop();
                
                // Проверяем, что это вражеский корабль (Ship)
                if (typeid(*troop) == typeid(gl::Ship) && 
                    isEnemy(troop->getOwner(), gl::Owner(&pirate))) {
                    
                    // Вычисляем расстояние для определения ближайшего врага
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
            
            // 4. Если не хватает дальности атаки - приближаемся
            std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                *currentHex, hexMap, pirateTroop->getMoveRange(), gl::RangeMode::MOVE);
            
            // Ищем лучшую клетку для приближения к врагу
            gl::Hex* bestMoveHex = nullptr;
            int bestDistance = pirateTroop->getView() + 1;
            
            for (gl::Hex* moveHex : moveRangeHexes) {
                // Проверяем, что клетка свободна и пират может туда пойти
                if (!moveHex->hasTroop() && !moveHex->hasBuilding() && 
                    pirateTroop->canMoveTo(moveHex)) {
                    int distanceToEnemy = hexDistance(*moveHex, *closestEnemy);
                    if (distanceToEnemy < bestDistance) {
                        bestDistance = distanceToEnemy;
                        bestMoveHex = moveHex;
                    }
                }
            }
            
            // Если нашли подходящую клетку - двигаемся
            if (bestMoveHex != nullptr) {
                pirateTroop->moveTo(bestMoveHex);
                pirateTroop->takeGoldFromCell(bestMoveHex);
                continue;
            }
        }
        
        // 5. Если нет врагов в радиусе видимости - ищем клетку с максимальным золотом
        std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
            *currentHex, hexMap, pirateTroop->getMoveRange(), gl::RangeMode::MOVE);
        
        // Фильтруем доступные клетки и ищем ту, где больше всего золота
        std::vector<std::pair<gl::Hex*, int>> availableMovesWithGold; // клетка -> количество золота
        
        for (gl::Hex* moveHex : moveRangeHexes) {
            if (!moveHex->hasTroop() && !moveHex->hasBuilding() && 
                pirateTroop->canMoveTo(moveHex)) {
                
                int goldAmount = moveHex->getGold(); // Предполагаем, что есть такой метод
                availableMovesWithGold.emplace_back(moveHex, goldAmount);
            }
        }
        
        if (!availableMovesWithGold.empty()) {
            // Сортируем по убыванию золота
            std::sort(availableMovesWithGold.begin(), availableMovesWithGold.end(),
                [](const std::pair<gl::Hex*, int>& a, const std::pair<gl::Hex*, int>& b) {
                    return a.second > b.second; // больше золота -> выше приоритет
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
        
        // Определяем тип войска
        bool isShip = (typeid(*enemyTroop) == typeid(gl::Ship));
        
        // 1. Получаем клетки видимости вокруг врага
        std::vector<gl::Hex*> visibleHexes = cellsInRange(
            *currentHex, hexMap, enemyTroop->getView(), gl::RangeMode::VIEW);
        
        // ПРИОРИТЕТ 1: Если у врага нет предмета - ищем предметы в видимости
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
                // Если предмет в соседней клетке - берем его
                if (hexDistance(*currentHex, *closestItemHex) <= 1) {
                    enemyTroop->takeItemByIndexFromCell(0);
                    continue;
                }
                
                // Иначе двигаемся к предмету
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
        
        // ПРИОРИТЕТ 2: Если есть предмет - возвращаемся в порт
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
                
                // Двигаемся к порту
                std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
                    *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
                
                gl::Hex* bestMoveHex = nullptr;
                int bestDistance = minPortDistance;
                
                for (gl::Hex* moveHex : moveRangeHexes) {
                    // Для движения к порту разрешаем занимать клетку с портом
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
        
        // ПРИОРИТЕТ 3: Ищем вражеские войска соответствующего типа для атаки
        gl::Hex* closestEnemyTroopHex = nullptr;
        int minEnemyDistance = enemyTroop->getView() + 1;
        
        for (gl::Hex* hex : visibleHexes) {
            if (hex->hasTroop()) {
                gl::Troop* troop = hex->getTroop();
                
                if (isEnemy(troop->getOwner(), gl::Owner(&enemy))) {
                    bool troopIsShip = (typeid(*troop) == typeid(gl::Ship));
                    
                    // Корабли атакуют только корабли, солдаты - только солдат
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
            
            // Если враг в радиусе атаки - атакуем
            if (std::find(attackRangeHexes.begin(), attackRangeHexes.end(), closestEnemyTroopHex) 
                != attackRangeHexes.end()) {
                
                gl::Troop* enemyTroopTarget = closestEnemyTroopHex->getTroop();
                if (enemyTroopTarget) {
                    enemyTroop->giveDamageToTroop(closestEnemyTroopHex);
                    continue;
                }
            }
            
            // Если не хватает дальности атаки - приближаемся
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
        
        // ПРИОРИТЕТ 4: Ищем клетку с максимальным золотом
        std::vector<gl::Hex*> moveRangeHexes = cellsInRange(
            *currentHex, hexMap, enemyTroop->getMoveRange(), gl::RangeMode::MOVE);
        
        // Собираем информацию о золоте на доступных клетках
        std::vector<std::pair<gl::Hex*, int>> availableMovesWithGold;
        
        for (gl::Hex* moveHex : moveRangeHexes) {
            if (!moveHex->hasTroop() && enemyTroop->canMoveTo(moveHex)) {
                // Проверяем золото на клетке
                int goldAmount = 0;
                
                // Если есть метод getGold(), используем его
                // goldAmount = moveHex->getGold();
                
                // Или проверяем ресурсы на клетке
                // if (moveHex->hasResource() && moveHex->getResource()->getType() == ResourceType::Gold) {
                //     goldAmount = moveHex->getResource()->getAmount();
                // }
                
                availableMovesWithGold.emplace_back(moveHex, goldAmount);
            }
        }
        
        if (!availableMovesWithGold.empty()) {
            // Сортируем по убыванию золота
            std::sort(availableMovesWithGold.begin(), availableMovesWithGold.end(),
                [](const std::pair<gl::Hex*, int>& a, const std::pair<gl::Hex*, int>& b) {
                    return a.second > b.second;
                });
            
            // Выбираем клетку с максимальным золотом
            std::vector<gl::Hex*> bestGoldCells;
            int maxGold = availableMovesWithGold[0].second;
            
            for (const auto& [hex, gold] : availableMovesWithGold) {
                if (gold == maxGold) {
                    bestGoldCells.push_back(hex);
                } else {
                    break;
                }
            }
            
            // Случайный выбор среди клеток с максимальным золотом
            int randomIndex = bestGoldCells.size() > 1 ? rand() % bestGoldCells.size() : 0;
            gl::Hex* targetHex = bestGoldCells[randomIndex];
            
            enemyTroop->moveTo(targetHex);
            enemyTroop->takeGoldFromCell(targetHex); // Если есть такой метод
            continue;
        }
        
        // ПРИОРИТЕТ 5: Случайное перемещение (если нет других целей)
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
