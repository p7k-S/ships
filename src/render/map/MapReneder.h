#pragma once
#include <SFML/Graphics.hpp>
#include <charconv>
#include <cmath>
#include "../../game/map.h"
#include "../../game/CubePerlin.h"

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

inline std::vector<GameLogic::Hex> createMap(const PerlinNoise& perlin, int mapWidth, int mapHeight, int octaves, const double scale)
{
    std::vector<GameLogic::Hex> hexMap;
    hexMap.reserve(mapWidth * mapHeight);

    for (int r = 0; r < mapHeight; ++r) {
        for (int q = 0; q < mapWidth; ++q) {
            double val = perlin.octaveNoise(q * scale, r * scale, octaves, 0.5);
            // if (q == mapWidth - 1 || r == mapHeight - 1) val = 0.5;
            hexMap.emplace_back(GameLogic::Hex(q, r, val));
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
