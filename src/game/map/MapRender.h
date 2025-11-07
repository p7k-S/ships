#include <SFML/Graphics.hpp>
#include <cmath>
#include "Cell.h"
#include "PerlinNoise.h"
#include "../GameConfig.h"


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

// void renderRangeHex(gl::Hex* hex, sf::Color fillColor, sf::Color outlineColor) {
//     double x_pos = hex->q * hexRadius * 1.5;
//     double y_pos = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0;
//
//     sf::ConvexShape rangeShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
//     rangeShape.setFillColor(fillColor);
//     rangeShape.setOutlineColor(outlineColor);
//     rangeShape.setOutlineThickness(1);
//     window.draw(rangeShape);
// }
