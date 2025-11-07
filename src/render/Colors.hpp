#pragma once
#include <SFML/Graphics.hpp>
#include "../game/GameConfig.h"


inline sf::Color getColorByThreshold_view(double value, double deepWater, double water, double land) {
    if (value <= deepWater) return sf::Color(0x0245ABFF); // deepWater
    if (value <= water) return sf::Color(0x1E5AC8FF);    // water
    if (value <= land) return sf::Color(0x3D7A51FF);     // land 0xA08C5AFF
    return sf::Color(0x2D6441FF);                        // forest
}
inline sf::Color getColorByThreshold_explored(double value, double deepWater, double water, double land) {
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
            return getColorByThreshold_explored(value, deepWater, water, land);
        default:
            return getColorByThreshold_view(value, deepWater, water, land);
    }
}

// Пример использования:
// sf::Color getColor(double value, double deepWater, double water, double land) {
//     return getColorByScheme(value, defaultScheme, deepWater, water, land);
// }
