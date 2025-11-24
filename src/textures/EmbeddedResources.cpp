#include "EmbeddedResources.h"
#include "embedded_resources.h"

// Определяем статические текстуры
sf::Texture EmbeddedResources::player_ship_texture;
sf::Texture EmbeddedResources::pirate_ship_texture;
sf::Texture EmbeddedResources::enemy_ship_texture;
sf::Texture EmbeddedResources::port_texture;
sf::Texture EmbeddedResources::gold_texture;
sf::Texture EmbeddedResources::treasure_texture;

// Определяем статический шрифт
sf::Font EmbeddedResources::main_font;

bool EmbeddedResources::loadTexture(sf::Texture& texture, const unsigned char* data, unsigned int size, const char* name) {
    if (texture.loadFromMemory(data, size)) {
        // printf("✅ Texture %s: %dx%d, %u bytes\n", name, 
               // texture.getSize().x, texture.getSize().y, size);
        return true;
    } else {
        // printf("❌ FAILED to load texture %s\n", name);
        return false;
    }
}

bool EmbeddedResources::loadFont(sf::Font& font, const unsigned char* data, unsigned int size, const char* name) {
    if (font.loadFromMemory(data, size)) {
        // printf("✅ Font %s: %u bytes\n", name, size);
        return true;
    } else {
        // printf("❌ FAILED to load font %s\n", name);
        return false;
    }
}

bool EmbeddedResources::loadAllResources() {
    // printf("=== Loading Embedded Resources ===\n");
    
    bool success = true;
    
    // Загружаем текстуры
    if (!loadTexture(player_ship_texture, player_ship_png, player_ship_png_len, "player_ship")) {
        success = false;
    }
    if (!loadTexture(pirate_ship_texture, pirates_ship_png, pirates_ship_png_len, "pirates_ship")) {
        success = false;
    }
    if (!loadTexture(enemy_ship_texture, enemy_ship_png, enemy_ship_png_len, "enemy_ship")) {
        success = false;
    }
    if (!loadTexture(port_texture, port_png, port_png_len, "enemy_ship")) {
        success = false;
    }
    if (!loadTexture(gold_texture, gold_cons_png, gold_cons_png_len, "gold_cons")) {
        success = false;
    }
    if (!loadTexture(treasure_texture, treasure_png, treasure_png_len, "treasure")) {
        success = false;
    }
    
    // Загружаем шрифты
    if (!loadFont(main_font, JetBrainsMonoNLNerdFont_Regular_ttf, JetBrainsMonoNLNerdFont_Regular_ttf_len, "main_font")) {
        success = false;
    }
    
    printf("=== Resource loading %s ===\n", success ? "SUCCESS" : "FAILED");
    return success;
}
