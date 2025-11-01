#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, sf::Color> MAP_COLORS = {
    {"gray", sf::Color(128, 128, 128)},
    {"dark_gray", sf::Color(80, 80, 80)},
    {"deep_yellow", sf::Color(255, 200, 50)},
    {"burgundy", sf::Color(150, 30, 70)},
    {"dark_green", sf::Color(2, 124, 2)},
    {"reachable_border", sf::Color(2, 124, 2)}, // borders of reachable cells
    {"white", sf::Color(255, 255, 255)},
    {"very_dark_gray", sf::Color(40, 40, 40)},
    {"yellow", sf::Color(255, 255, 0)},
    {"red", sf::Color(255, 0, 0)},
    {"green", sf::Color(0, 255, 0)},
    {"blue", sf::Color(0, 0, 255)},
    {"yellow2", sf::Color(255, 255, 0)},
    {"magenta", sf::Color(255, 0, 255)},
    {"cyan", sf::Color(0, 255, 255)},
    {"dark_red", sf::Color(128, 0, 0)},
    {"dark_green2", sf::Color(0, 128, 0)}
};

enum ColorScheme {
    COLORS,
    DARK_COLORS,
    INVERT
};

ColorScheme colScheme = COLORS;
ColorScheme colSchemeInactive = DARK_COLORS;

inline sf::Color getColorByThreshold(double value, double deepWater, double water, double land) {
    if (value <= deepWater) return sf::Color(0x0245ABFF); // deepWater
    if (value <= water) return sf::Color(0x1E5AC8FF);    // water
    if (value <= land) return sf::Color(0x3D7A51FF);     // land 0xA08C5AFF
    return sf::Color(0x2D6441FF);                        // forest
}
inline sf::Color getColorByThresholdDark(double value, double deepWater, double water, double land) {
    if (value <= deepWater) return sf::Color(0x091A3CFF); // темный deepWater + серый
    if (value <= water) return sf::Color(0x1A2645FF);     // темный water + серый
    if (value <= land) return sf::Color(0x2E3F2DFF);      // темный land + серый
    return sf::Color(0x273529FF);                         // темный forest + серый
}

inline sf::Color getGrayscaleColor(double value) {
    int colorValue = static_cast<int>(value * 255);
    return sf::Color(colorValue, colorValue, colorValue);
}

inline sf::Color getColorByScheme(double value, ColorScheme scheme, double deepWater, double water, double land) {
    switch (scheme) {
        case ColorScheme::INVERT:
            return getGrayscaleColor(value);
        case ColorScheme::DARK_COLORS:
            return getColorByThresholdDark(value, deepWater, water, land);
        default:
            return getColorByThreshold(value, deepWater, water, land);
    }
}

// Пример использования:
// sf::Color getColor(double value, double deepWater, double water, double land) {
//     return getColorByScheme(value, defaultScheme, deepWater, water, land);
// }
