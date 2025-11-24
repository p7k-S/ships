#include <SFML/Graphics.hpp>
#include <cmath>
#include "Cell.h"
#include "PerlinNoise.h"

sf::ConvexShape createHex(double x, double y, double radius) {
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

std::vector<GameLogic::Hex> createMap(const PerlinNoise& perlin, int mapWidth, int mapHeight, int octaves, const double scale)
{
    std::vector<GameLogic::Hex> hexMap;
    hexMap.reserve(mapWidth * mapHeight);

    for (int r = 0; r < mapHeight; ++r) {
        for (int q = 0; q < mapWidth; ++q) {
            double val = perlin.octaveNoise(q * scale, r * scale, octaves, 0.5);
            hexMap.emplace_back(GameLogic::Hex(q, r, val));
        }
    }

    return hexMap;
}

std::vector<double> getNoises(std::vector<GameLogic::Hex>& hexMap){
    std::vector<double> noises;
    for (const auto& hex : hexMap) {
        noises.push_back(hex.getNoise());
    }
    return noises;
}

// void distributeCellTypes(std::vector<GameLogic::Hex> hexMap) {
//     // Определение уровней высот для типов клеток
//     std::vector<double> noiseValues = getNoises(hexMap);
//     std::vector<double> sortedValues = noiseValues;
//     std::sort(sortedValues.begin(), sortedValues.end());
//
//     double deepWater = sortedValues[noiseValues.size() * deepWater_delim];
//     double water = sortedValues[noiseValues.size() * water_delim];
//     double land = sortedValues[noiseValues.size() * land_delim];
//
//     // Распределение клеток по типам
//     for (auto& hex : hexMap) {
//         double value = hex.getNoise();
//         CellType type;
//
//         if (value <= deepWater) {
//             type = gl::CellType::DEEPWATER;
//         } else if (value <= water) {
//             type = gl::CellType::WATER;
//         } else if (value <= land) {
//             type = gl::CellType::LAND;
//         } else {
//             type = gl::CellType::FOREST;
//         }
//         hex.setCellType(type);
//     }
// }
