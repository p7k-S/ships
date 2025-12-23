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
