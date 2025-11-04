#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "../game/map.h"
namespace gl = GameLogic;

inline bool pointInHex(float px, float py, float hx, float hy, float radius) {
    float dx = px - hx;
    float dy = py - hy;
    float distance = sqrt(dx*dx + dy*dy);
    return distance <= radius;
}

inline gl::Hex* getHexAtPosition(int mouseX, int mouseY, std::vector<gl::Hex>& hexMap, double hexRadius) {
    for (auto& hex : hexMap) {
        auto pos = hex.getPosition(hexRadius); // Метод должен возвращать координаты клетки в пикселях

        // Проверим, попала ли точка в клетку
        double dx = mouseX - pos.x;
        double dy = mouseY - pos.y;

        // Тут мы проверяем, попала ли мышь на этот шестиугольник.
        if (std::abs(dx) < hexRadius * 1.5 && std::abs(dy) < hexRadius * sqrt(3)) {
            return &hex;
        }
    }
    return nullptr; // если ничего не нашли
}
