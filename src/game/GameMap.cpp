#include "Game.h"
#include "map/GenerateMap.h"
#include "GameConfig.h"
#include "../render/info_bars.h"
// #include "items/Gold.h"
#include "items/Treasure.h"
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

void Game::createShips() {
    std::vector<gl::Hex*> waterCells;
    for (auto& hex : hexMap) {
        if (hex.getCellType() == gl::CellType::WATER) {
            waterCells.push_back(&hex);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(waterCells.begin(), waterCells.end(), g);

    for (size_t i = 0; i < playersAmount; ++i) {
        gl::Owner owner = players[i].get();
        auto ship = std::make_unique<gl::Ship>(owner, waterCells[i]);
        gl::Ship* shipPtr = ship.get();
        waterCells[i]->setTroopOf<gl::Ship>(shipPtr);
        players[i]->addTroop(std::move(ship));

        addViewedCells(players[i]->getSeenCells(), shipPtr, hexMap, gl::RangeMode::VIEW);
    }

    for (size_t i = playersAmount; i - playersAmount < waterCells.size() * percent_ships_in_water; ++i) {
        gl::Owner owner = (i % 2) ? static_cast<gl::Owner>(&pirate) : static_cast<gl::Owner>(&enemy);
        auto ship = std::make_unique<gl::Ship>(owner, waterCells[i]);
        gl::Ship* shipPtr = ship.get();
        waterCells[i]->setTroopOf<gl::Ship>(shipPtr);

        if (i % 2) {
            pirate.addTroop(std::move(ship));
        } else {
            enemy.addTroop(std::move(ship));
        }
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
