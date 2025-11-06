#include "Game.h"
#include <algorithm>

void Game::generateMap() {
    PerlinNoise perlin(seed);
    hexMap = createMap(perlin, mapWidth, mapHeight, octaves, scale);
}

void Game::distributeCellTypes() {
    std::vector<double> noiseValues = getNoises(hexMap);
    std::sort(noiseValues.begin(), noiseValues.end());
    // логика распределения типов клеток...
}

void Game::placeGoldAndTreasures() {
    std::uniform_int_distribution<> goldDist(10, 100);
    std::uniform_real_distribution<> chanceDist(0.0, 1.0);
    // логика размещения золота и сокровищ...
}

