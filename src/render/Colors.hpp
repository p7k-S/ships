#pragma once
#include <SFML/Graphics.hpp>

const std::vector<sf::Color> MAP_COLORS = {
    sf::Color(128, 128, 128),  // Серый
    sf::Color(80, 80, 80),  // Серый
    sf::Color(255, 200, 50),  // deep yellow
    sf::Color(150, 30, 70),  // бордовый
    sf::Color(2, 124, 2),  // dark green active ship
    sf::Color(2, 124, 2),  // borders of reachable cells
    sf::Color(255, 255, 255),  // white
    sf::Color(255, 255, 0),    // Желтый
    sf::Color(255, 0, 0),      // Красный
    sf::Color(0, 255, 0),      // Зеленый
    sf::Color(0, 0, 255),      // Синий
    sf::Color(255, 255, 0),    // Желтый
    sf::Color(255, 0, 255),    // Пурпурный
    sf::Color(0, 255, 255),    // Голубой
    sf::Color(128, 0, 0),      // Темно-красный
    sf::Color(0, 128, 0)       // Темно-зеленый
};

enum ColorScheme {
    COLORS,
    INVERT
};

ColorScheme colScheme = COLORS;

inline sf::Color getColorByThreshold(double value, double deepWater, double water, double land) {
    if (value <= deepWater) return sf::Color(0x0245ABFF); // deepWater
    if (value <= water) return sf::Color(0x1E5AC8FF);    // water
    if (value <= land) return sf::Color(0x3D7A51FF);     // land 0xA08C5AFF
    return sf::Color(0x2D6441FF);                        // forest
}

inline sf::Color getGrayscaleColor(double value) {
    int colorValue = static_cast<int>(value * 255);
    return sf::Color(colorValue, colorValue, colorValue);
}

inline sf::Color getColorByScheme(double value, ColorScheme scheme, double deepWater, double water, double land) {
    switch (scheme) {
        case ColorScheme::INVERT:
            return getGrayscaleColor(value);
        case ColorScheme::COLORS:
        default:
            return getColorByThreshold(value, deepWater, water, land);
    }
}

// Пример использования:
// sf::Color getColor(double value, double deepWater, double water, double land) {
//     return getColorByScheme(value, defaultScheme, deepWater, water, land);
// }
