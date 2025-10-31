#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "../../game/map.h"
#include "CubePerlin.h"

// --- Функция создания гекса для SFML ---
inline sf::ConvexShape createHex(double x, double y, double radius) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3.0 * i;
        double px = x + radius * std::cos(angle);
        double py = y + radius * std::sin(angle);
        hex.setPoint(i, sf::Vector2f(px, py));
    }
    return hex;
}

// --- Перевод гекса в кубические координаты ---
inline void axialToCube(int q, int r, int& x, int& y, int& z) {
    x = q;
    z = r;
    y = -x - z;
}

inline std::vector<GameLogic::Hex> createMap(PerlinNoise perlin, int mapWidth, int mapHeight, int octaves) {
    std::vector<GameLogic::Hex> hexMap;

    for (int cx, cy, cz, r = 0; r < mapHeight; r++) {
        for (int q = 0; q < mapWidth; q++) {
            axialToCube(q, r, cx, cy, cz);
            double value = perlin.octaveNoise(cx * 0.1, cy * 0.1, cz * 0.1, octaves, 0.5);
            hexMap.push_back(GameLogic::Hex(q, r, value));
        }
    }
    return hexMap;
}

inline std::vector<double> getNoises(std::vector<GameLogic::Hex> hexMap){
    std::vector<double> noises;
    for (const auto hex : hexMap) {
        noises.push_back(hex.getNoise());
    }
    return noises;
}

inline std::vector<double> getlandscapehex(std::vector<GameLogic::Hex> hexMap){
    std::vector<double> noises;
    for (const auto hex : hexMap) {
        noises.push_back(hex.getNoise());
    }
    return noises;
}
