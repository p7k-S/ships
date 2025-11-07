#pragma once
#include "../GameConfig.h"

namespace GameLogic {
    class Entity {
        protected: 
            sf::Color color;

        public:
            Entity(sf::Color col) : color(col) {}

            sf::Color getColor() const { return color; }

            void setColor(sf::Color& newColor) { color = newColor; }
    };
}; // namespace GameLogic
