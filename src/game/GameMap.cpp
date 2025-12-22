#include "Game.h"
#include "map/GenerateMap.h"
#include "GameConfig.h"
#include "../render/info_bars.h"
// #include "items/Gold.h"
#include "items/Treasure.h"
#include "buildings/Port.h" // или правильный путь к Port.h
#include "troops/Soldier.h"
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <random>
// #include <iostream>

void Game::generateMap() {
    PerlinNoise perlin(seed);
    hexMap = createMap(perlin, mapWidth, mapHeight, octaves, scale);
}

// HexBiome getBiomeWithSmoothing(double noiseValue, const BiomeThresholds& thresholds, 
//                               double transitionWidth = 0.05) {
//     // Плавные переходы между биомами
//     if (noiseValue < thresholds.deepWater - transitionWidth) 
//         return HexBiome::DEEP_WATER;
//
//     if (noiseValue < thresholds.deepWater + transitionWidth) {
//         // Случайный выбор между глубокой водой и водой для плавного перехода
//         double blend = (noiseValue - (thresholds.deepWater - transitionWidth)) / (2 * transitionWidth);
//         return (random() < blend) ? HexBiome::WATER : HexBiome::DEEP_WATER;
//     }
//
//     if (noiseValue < thresholds.water - transitionWidth) 
//         return HexBiome::WATER;
//
//     if (noiseValue < thresholds.water + transitionWidth) {
//         double blend = (noiseValue - (thresholds.water - transitionWidth)) / (2 * transitionWidth);
//         return (random() < blend) ? HexBiome::LAND : HexBiome::WATER;
//     }
//
//     // ... и так для остальных границ
//     return HexBiome::LAND;
// }

void Game::distributeCellTypes() {
    std::vector<double> noiseValues = getNoises(hexMap);
    std::vector<double> sortedValues = noiseValues;
    std::sort(sortedValues.begin(), sortedValues.end());

    deepWater = sortedValues[noiseValues.size() * deepWater_delim];
    water = sortedValues[noiseValues.size() * water_delim];
    land = sortedValues[noiseValues.size() * land_delim];

    for (auto& hex : hexMap) {
        double value = hex.getNoise();
        gl::CellType type;

        if (value <= deepWater) {
            type = gl::CellType::DEEPWATER;
            // deepWaterHexes.push_back(&hex);
        } else if (value <= water) {
            type = gl::CellType::WATER;
            // waterHexes.push_back(&hex);
        } else if (value <= land) {
            type = gl::CellType::LAND;
            // landHexes.push_back(&hex);
        } else {
            type = gl::CellType::FOREST;
            // forestHexes.push_back(&hex);
        }

        hex.setCellType(type);
    }
}

void Game::createPlayers() {
    if (isNetworkGame) {
        std::cout << "Network game: players created via network" << std::endl;
        return;
    }
    
    for (uint8_t i = 0; i < playersAmount; ++i) {
        std::string name, color;
        std::cout << "Enter name for player " << i + 1 << ": ";
        std::cin >> name;
        std::cout << "Enter color for player " << i + 1 << ": ";
        std::cin >> color;

        auto player = std::make_unique<gl::Player>(name, COLORS[color]);
        players.push_back(std::move(player));
    }
}

bool Game::portCanPlayced(const gl::Hex& h) {
    if (h.getCellType() != gl::CellType::WATER) {
        return false;
    }

    auto neighborCoords = gl::getNeighbors(h);
    bool hasLandNeighbor = false;

    for (auto& neighborHex : neighborCoords) {
        int q = neighborHex.q;
        int r = neighborHex.r;

        // Проверяем границы
        if (q >= 0 && q < mapWidth && r >= 0 && r < mapHeight) {
            int index = q + r * mapWidth;
            if (index >= 0 && index < hexMap.size()) {
                if (hexMap[index].getCellType() == gl::CellType::LAND) {
                    hasLandNeighbor = true;
                    break; // Достаточно одного соседа с LAND
                }
            }
        }
        // Если клетка за границами - игнорируем её
    }

    return hasLandNeighbor;
}

bool Game::placeShip(const gl::Hex& portCell) {
    gl::Hex* cell = nullptr;
    const int RAD = 10;
    bool found = false;

    for (int dist = 1; dist <= RAD && !found; dist++) {
        for (int r = portCell.r - dist; r <= portCell.r + dist && !found; r++) {
            for (int q = portCell.q - dist; q <= portCell.q + dist && !found; q++) {
                if (q == portCell.q && r == portCell.r) continue;

                // Быстрая проверка - если оба смещения меньше dist, это внутренняя клетка
                if (abs(q - portCell.q) < dist && abs(r - portCell.r) < dist) continue;

                if (q >= 0 && q < mapWidth && r >= 0 && r < mapHeight) {
                    int i = q + r * mapWidth;
                    if (i >= 0 && i < hexMap.size()) {
                        if (hexMap[i].getCellType() <= gl::CellType::WATER && 
                                !hexMap[i].hasTroop() && 
                                !hexMap[i].hasBuilding()) {
                            cell = &hexMap[i];
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    }


    if (cell) {
        auto ship = std::make_unique<gl::Ship>(players[p_id].get(), cell);
        gl::Ship* shipPtr = ship.get();
        cell->setTroopOf<gl::Ship>(shipPtr);
        players[p_id]->addTroop(std::move(ship));

        return true;
    }

    return true;
}

bool Game::placeSoldier(const gl::Hex& portCell) {
    gl::Hex* cell = nullptr;
    const int RAD = 10;
    bool found = false;

    for (int dist = 1; dist <= RAD && !found; dist++) {
        for (int r = portCell.r - dist; r <= portCell.r + dist && !found; r++) {
            for (int q = portCell.q - dist; q <= portCell.q + dist && !found; q++) {
                if (q == portCell.q && r == portCell.r) continue;

                // Быстрая проверка - если оба смещения меньше dist, это внутренняя клетка
                if (abs(q - portCell.q) < dist && abs(r - portCell.r) < dist) continue;

                if (q >= 0 && q < mapWidth && r >= 0 && r < mapHeight) {
                    int i = q + r * mapWidth;
                    if (i >= 0 && i < hexMap.size()) {
                        if (hexMap[i].getCellType() >= gl::CellType::LAND && 
                                !hexMap[i].hasTroop() && 
                                !hexMap[i].hasBuilding()) {
                            cell = &hexMap[i];
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (cell) {
        auto soldier = std::make_unique<gl::Soldier>(players[p_id].get(), cell);
        gl::Soldier* soldPtr = soldier.get();
        cell->setTroopOf<gl::Soldier>(soldPtr);
        players[p_id]->addTroop(std::move(soldier));

        return true;
    }

    return true;
}

void Game::createTroops() {
    std::vector<gl::Hex*> waterCells;
    for (auto& hex : hexMap) {
        if (hex.getCellType() == gl::CellType::WATER && portCanPlayced(hex)) {
            waterCells.push_back(&hex);
        }
    }

    std::vector<std::vector<gl::Hex*>> sectors(playersAmount);
    for (size_t i = 0; i < waterCells.size(); ++i) {
        int sector = (i * playersAmount) / waterCells.size();
        sectors[sector].push_back(waterCells[i]);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < playersAmount; ++i) {
        if (!sectors[i].empty()) {
            std::shuffle(sectors[i].begin(), sectors[i].end(), g);
            gl::Hex* selectedHex = sectors[i][0];

            std::cout << "DEBUG: Creating port for player " << i 
                << " at (" << selectedHex->q << "," << selectedHex->r << ")" << std::endl;
            std::cout << "DEBUG: Hex already has building: " << selectedHex->hasBuilding() << std::endl;
            std::cout << "DEBUG: Hex already has troop: " << selectedHex->hasTroop() << std::endl;

            gl::Owner owner = players[i].get();
            auto port = std::make_unique<gl::Port>(owner, selectedHex);
            gl::Port* portPtr = port.get();

            auto neighborCoords = gl::getNeighbors(*selectedHex);
            for (auto& neighborHex : neighborCoords) {
                int q = neighborHex.q;
                int r = neighborHex.r;

                // Проверяем границы как в portCanPlayced
                if (q >= 0 && q < mapWidth && r >= 0 && r < mapHeight) {
                    int index = q + r * mapWidth;
                    if (index >= 0 && index < hexMap.size() && 
                            hexMap[index].getCellType() == gl::CellType::LAND) {

                        // Размещаем Soldier на ЗЕМЛЕ, а не на воде!
                        auto soldier = std::make_unique<gl::Soldier>(owner, &hexMap[index]);
                        gl::Soldier* soldierPtr = soldier.get();

                        if (hexMap[index].setTroopOf<gl::Soldier>(soldierPtr)) {
                            players[i]->addTroop(std::move(soldier));
                            // addViewedCells(players[i]->getSeenCells(), soldier, hexMap, gl::RangeMode::VIEW);
                            // addViewedCells(players[i]->getViewableCells(), soldier, hexMap, gl::RangeMode::VIEW);
                            break;
                        }
                    }
                }
            }

            if (selectedHex->setBuildingOf<gl::Port>(portPtr)) {
                // addViewedCells(players[i]->getSeenCells(), port, hexMap, gl::RangeMode::VIEW);
                // addViewedCells(players[i]->getViewableCells(), port, hexMap, gl::RangeMode::VIEW);

                players[i]->addBuilding(std::move(port));

                auto ship = std::make_unique<gl::Ship>(owner, selectedHex);
                gl::Ship* shipPtr = ship.get();

                if (selectedHex->setTroopOf<gl::Ship>(shipPtr)) {
                    players[i]->addTroop(std::move(ship));
                    std::cout << "DEBUG: Player " << i << " now has " << players[i]->getTroops().size() << " troops" << std::endl;

                    addViewedCells(players[i]->getSeenCells(), shipPtr, hexMap, gl::RangeMode::VIEW);
                    // это нужно т.к. хост создает и id всегда хостовое
                    addViewedCells(players[i]->getViewableCells(), shipPtr, hexMap, gl::RangeMode::VIEW);
                }

                sectors[i].erase(sectors[i].begin());
            } else {
                std::cout << "DEBUG: FAILED to create port for player " << i << std::endl;
            }
        } else {
            std::cout << "DEBUG: No cells for player " << i << std::endl;
        }
    }

    // 1. СОБИРАЕМ все оставшиеся клетки из sectors
    std::vector<gl::Hex*> remainingCells;
    for (auto& sector : sectors) {
        remainingCells.insert(remainingCells.end(), sector.begin(), sector.end());
    }

    // 2. ВЫЧИСЛЯЕМ количество секторов для NPC
    size_t npcSectorCount = static_cast<size_t>(remainingCells.size() * percent_ships_in_water);
    npcSectorCount = std::max(size_t(1), std::min(npcSectorCount, remainingCells.size()));

    std::vector<std::vector<gl::Hex*>> npcSectors(npcSectorCount);
    for (size_t i = 0; i < remainingCells.size(); ++i) {
        int sector = (i * npcSectorCount) / remainingCells.size();
        npcSectors[sector].push_back(remainingCells[i]);
    }


    for (int i = 0; i < npcSectors.size(); ++i) { // *2 тк к каждому в его сектор добавим по enemy
            std::shuffle(npcSectors[i].begin(), npcSectors[i].end(), g);
            gl::Hex* selectedHex = npcSectors[i][0];

            gl::Owner owner = static_cast<gl::Owner>(&enemy);
            auto port = std::make_unique<gl::Port>(owner, selectedHex);
            gl::Port* portPtr = port.get();
            auto neighborCoords = gl::getNeighbors(*selectedHex);
            for (auto& neighborHex : neighborCoords) {
                int q = neighborHex.q;
                int r = neighborHex.r;

                // Проверяем границы как в portCanPlayced
                if (q >= 0 && q < mapWidth && r >= 0 && r < mapHeight) {
                    int index = q + r * mapWidth;
                    if (index >= 0 && index < hexMap.size() && 
                            hexMap[index].getCellType() == gl::CellType::LAND) {

                        // Размещаем Soldier на ЗЕМЛЕ, а не на воде!
                        auto soldier = std::make_unique<gl::Soldier>(owner, &hexMap[index]);
                        gl::Soldier* soldierPtr = soldier.get();

                        if (hexMap[index].setTroopOf<gl::Soldier>(soldierPtr)) {
                            enemy.addTroop(std::move(soldier));
                            break;
                        }
                    }
                }
            }
            if (selectedHex->setBuildingOf<gl::Port>(portPtr)) {
                enemy.addBuilding(std::move(port));
                npcSectors[i].erase(npcSectors[i].begin());
            }
    }


    std::vector<gl::Hex*> allWaterCells;
    for (auto& hex : hexMap) {
        if (hex.getCellType() == gl::CellType::DEEPWATER || hex.getCellType() == gl::CellType::WATER) {
            if (!hex.hasTroop())
                allWaterCells.push_back(&hex);
        }
    }

    std::shuffle(allWaterCells.begin(), allWaterCells.end(), g);

    for (size_t i = playersAmount; i - playersAmount < allWaterCells.size() * percent_ships_in_water; ++i) {
        gl::Owner owner = (i % 2) ? static_cast<gl::Owner>(&pirate) : static_cast<gl::Owner>(&enemy);
        auto ship = std::make_unique<gl::Ship>(owner, allWaterCells[i]);
        gl::Ship* shipPtr = ship.get();
        allWaterCells[i]->setTroopOf<gl::Ship>(shipPtr);

        if (i % 2) {
            pirate.addTroop(std::move(ship));
        } else {
            enemy.addTroop(std::move(ship));
        }

        allWaterCells.erase(allWaterCells.begin());
    }
}

void Game::placeGoldAndTreasures() {
    std::uniform_int_distribution<> goldDist(10, 100);
    std::uniform_real_distribution<> chanceDist(0.0, 1.0);

    for (auto& hex : hexMap)
        if (chanceDist(gen) < gold_perc_in_map)
            hex.addGold(goldDist(gen));

    int treasuresToPlace = treasuresAmount;
    std::uniform_int_distribution<> indexDist(0, hexMap.size() - 1);
    std::unordered_set<int> used;

    while (treasuresToPlace > 0 && used.size() < hexMap.size()) {
        int i = indexDist(gen);
        if (used.insert(i).second) {
            hexMap[i].addItem<gl::Treasure>("nice");
            treasuresToPlace--;
        }
    }
}
