#include <SFML/Graphics.hpp>

inline void normlaizeSprite(sf::Sprite&sprite, const double hexRadius, double x_pos,double y_pos){
    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    float scale = (hexRadius * 1.0f) / spriteBounds.width; // Меньше чем клетка
    sprite.setScale(scale, scale);
    sprite.setPosition(
            x_pos + 50 - spriteBounds.width * scale / 2,
            y_pos + 50 - spriteBounds.height * scale / 2
            );
}
