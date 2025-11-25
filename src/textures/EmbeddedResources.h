#pragma once
#include <SFML/Graphics.hpp>

// Все объявления теперь в одном файле
class EmbeddedResources {
public:
    static bool loadAllResources();
    
    // Текстуры
    static sf::Texture player_ship_texture;
    static sf::Texture player_soldier_texture;
    static sf::Texture pirate_ship_texture;
    static sf::Texture enemy_ship_texture;
    static sf::Texture enemy_soldier_texture;
    static sf::Texture port_texture;
    static sf::Texture gold_texture;
    static sf::Texture treasure_texture;
    
    // Шрифты
    static sf::Font main_font;
    
private:
    static bool loadTexture(sf::Texture& texture, const unsigned char* data, unsigned int size, const char* name);
    static bool loadFont(sf::Font& font, const unsigned char* data, unsigned int size, const char* name);
};
