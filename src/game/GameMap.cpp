#include "Game.h"
#include "map/GenerateMap.h"
#include "GameConfig.h"
#include "../render/info_bars.h"
#include <algorithm>
#include <unordered_set>
#include <random>
// #include <iostream>

void Game::generateMap() {
    PerlinNoise perlin(seed);
    hexMap = createMap(perlin, mapWidth, mapHeight, octaves, scale);
}

HexBiome getBiomeWithSmoothing(double noiseValue, const BiomeThresholds& thresholds, 
                              double transitionWidth = 0.05) {
    // Плавные переходы между биомами
    if (noiseValue < thresholds.deepWater - transitionWidth) 
        return HexBiome::DEEP_WATER;
    
    if (noiseValue < thresholds.deepWater + transitionWidth) {
        // Случайный выбор между глубокой водой и водой для плавного перехода
        double blend = (noiseValue - (thresholds.deepWater - transitionWidth)) / (2 * transitionWidth);
        return (random() < blend) ? HexBiome::WATER : HexBiome::DEEP_WATER;
    }
    
    if (noiseValue < thresholds.water - transitionWidth) 
        return HexBiome::WATER;
    
    if (noiseValue < thresholds.water + transitionWidth) {
        double blend = (noiseValue - (thresholds.water - transitionWidth)) / (2 * transitionWidth);
        return (random() < blend) ? HexBiome::LAND : HexBiome::WATER;
    }
    
    // ... и так для остальных границ
    return HexBiome::LAND;
}

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
            deepWaterHexes.push_back(&hex);
        } else if (value <= water) {
            type = gl::CellType::WATER;
            waterHexes.push_back(&hex);
        } else if (value <= land) {
            type = gl::CellType::LAND;
            landHexes.push_back(&hex);
        } else {
            type = gl::CellType::FOREST;
            forestHexes.push_back(&hex);
        }

        hex.setCellType(type);
    }
}

void Game::createShips() {
    std::srand(std::time(nullptr));
    for (int i = 0; i < (waterHexes.size() + deepWaterHexes.size()) * percent_ships_in_water; ++i) {
        gl::Owner owner = gl::Owner::PIRATE;
        if (i == 0) owner = gl::Owner::PLAYER;
        else if (i % 2) owner = gl::Owner::ENEMY;

        int randomIndex = std::rand() % waterHexes.size();
        gl::Hex* startHex = waterHexes[randomIndex];

        bool exists = false;
        for (const auto& ship : ships)
            if (ship->getCell() == startHex) { exists = true; break; }

        if (exists) { --i; continue; }

        auto myShip = std::make_unique<gl::Ship>(owner, startHex);
        startHex->setShip(myShip.get());
        ships.push_back(std::move(myShip));
    }

    if (!ships.empty())
        addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);
}

void Game::placeGoldAndTreasures() {
    std::uniform_int_distribution<> goldDist(10, 100);
    std::uniform_real_distribution<> chanceDist(0.0, 1.0);

    for (auto& hex : hexMap)
        if (chanceDist(gen) < gold_perc_in_map)
            hex.setGold(goldDist(gen));

    int treasuresToPlace = treasures;
    std::uniform_int_distribution<> indexDist(0, hexMap.size() - 1);
    std::unordered_set<int> used;

    while (treasuresToPlace > 0 && used.size() < hexMap.size()) {
        int i = indexDist(gen);
        if (used.insert(i).second) {
            hexMap[i].setTreasure("nice");
            treasuresToPlace--;
        }
    }
}
