#include "GameRenderer.h"
#include <SFML/Graphics.hpp>
#include "../game/constants.h"
#include "Colors.hpp"
#include <iostream>


GameRenderer(sf::RenderWindow& window, sf::Font& font) 
    : window_(window), font_(font), 
      colScheme_(COLORS), colSchemeInactive_(DARK_COLORS),
      deepWater_(0.0), water_(0.0), land_(0.0) {
}

bool loadTextures() {
    bool success = true;
    
    if (!player_ship_texture_.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/player_ship.png")) {
        std::cerr << "Failed to load player ship texture" << std::endl;
        success = false;
    }
    if (!pirate_ship_texture_.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/pirates_ship.png")) {
        std::cerr << "Failed to load pirate ship texture" << std::endl;
        success = false;
    }
    if (!enemy_ship_texture_.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/enemy_ship.png")) {
        std::cerr << "Failed to load enemy ship texture" << std::endl;
        success = false;
    }
    if (!gold_texture_.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/gold_cons.png")) {
        std::cerr << "Failed to load gold texture" << std::endl;
        success = false;
    }
    if (!treasure_texture_.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/treasure.png")) {
        std::cerr << "Failed to load treasure texture" << std::endl;
        success = false;
    }
    
    return success;
}

void setColorSchemes(ColorScheme active, ColorScheme inactive) {
    colScheme = active;
    colSchemeInactive = inactive;
}

void setTerrainThresholds(double deepWater, double water, double land) {
    deepWater = deepWater;
    water = water;
    land = land;
}

void renderMap(const std::vector<gl::Hex*>& seenCells, 
                           const std::vector<gl::Hex*>& vieweableHexes,
                           const std::vector<gl::Hex>& hexMap) {
    for (const auto& hexp : seenCells) {
        const auto& hex = *hexp;
        double x_pos = hex.q * hexRadius * 1.5;
        double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
        
        bool isVisible = std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end();
        renderHex(hex, x_pos, y_pos, isVisible);
    }
}

void renderHex(const gl::Hex& hex, float x_pos, float y_pos, bool isVisible) {
    sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
    
    // Установка цвета hex'а
    hexShape.setFillColor(getHexColor(hex, isVisible));
    renderHexOutline(hex, hexShape);
    
    window_.draw(hexShape);
    
    // Отрисовка объектов на hex'е
    if (isVisible) {
        renderShipOnHex(hex, x_pos, y_pos);
        renderResourcesOnHex(hex, x_pos, y_pos);
    }
}

void renderShipOnHex(const gl::Hex& hex, float x_pos, float y_pos) {
    if (hex.hasShip()) {
        sf::Sprite shipSprite;
        
        switch (hex.getShip()->getOwner()) {
            case gl::Owner::PIRATE:
                shipSprite.setTexture(pirate_ship_texture_);
                break;
            case gl::Owner::ENEMY:
                shipSprite.setTexture(enemy_ship_texture_);
                break;
            case gl::Owner::PLAYER:
                shipSprite.setTexture(player_ship_texture_);
                break;
            case gl::Owner::FRIENDLY:
                // Можно добавить текстуру для дружественных кораблей
                break;
        }
        
        normlaizeSprite(shipSprite, hexRadius, x_pos, y_pos);
        window_.draw(shipSprite);
    }
}

void renderResourcesOnHex(const gl::Hex& hex, float x_pos, float y_pos) {
    sf::Sprite resourceSprite;
    bool hasGold = false;
    
    if (hex.hasTreasure()) {
        resourceSprite.setTexture(treasure_texture_);
    } else if (hex.hasGold()) {
        resourceSprite.setTexture(gold_texture_);
        hasGold = true;
    }
    
    if (hex.hasGold() || hex.hasTreasure()) {
        normlaizeSprite(resourceSprite, hexRadius, x_pos, y_pos);
        window_.draw(resourceSprite);
    }
    
    if (hasGold && !hex.hasShip()) {
        drawResourceText(window_, hex, x_pos + 50, y_pos + 50, hexRadius, font_, font_size);
    }
}

void renderHexOutline(const gl::Hex& hex, sf::ConvexShape& hexShape) {
    if (colSchemeInactive_ == INVERT) {
        hexShape.setOutlineColor(MAP_COLORS["dark_gray"]);
    } else {
        hexShape.setOutlineColor(MAP_COLORS["very_dark_gray"]);
    }
    hexShape.setOutlineThickness(1);
    
    // Особые контуры для кораблей
    if (hex.hasShip()) {
        switch (hex.getShip()->getOwner()) {
            case gl::Owner::PIRATE:
                hexShape.setOutlineColor(sf::Color::Black);
                break;
            case gl::Owner::ENEMY:
                hexShape.setOutlineColor(MAP_COLORS["burgundy"]);
                break;
            case gl::Owner::PLAYER:
                hexShape.setOutlineColor(sf::Color::Green);
                break;
            case gl::Owner::FRIENDLY:
                hexShape.setOutlineColor(sf::Color::White);
                break;
        }
    }
}

sf::Color getHexColor(const gl::Hex& hex, bool isVisible) const {
    ColorScheme scheme = isVisible ? colScheme_ : colSchemeInactive_;
    
    // Специальные цвета для инверсной схемы с кораблями
    if (scheme == INVERT && hex.hasShip()) {
        switch (hex.getShip()->getOwner()) {
            case gl::Owner::PIRATE:
                return MAP_COLORS["very_dark_gray"];
            case gl::Owner::ENEMY:
                return MAP_COLORS["burgundy"];
            case gl::Owner::PLAYER:
                return MAP_COLORS["dark_green"];
            case gl::Owner::FRIENDLY:
                return sf::Color::White;
        }
    }
    
    return getColorByScheme(hex.getNoise(), scheme, deepWater_, water_, land_);
}

void renderMovementRange(gl::Ship* selectedShip, const std::vector<gl::Hex>& hexMap) {
    if (!selectedShip) return;
    
    gl::Hex* selectedHex = selectedShip->getCell();
    if (!selectedHex) return;
    
    std::vector<gl::Hex*> reachableHexes = selectedShip->cellsInRange(
        *selectedHex, hexMap, selectedShip->getMoveRange(), gl::RangeMode::MOVE);
    
    for (gl::Hex* reachableHex : reachableHexes) {
        double x_pos = reachableHex->q * hexRadius * 1.5;
        double y_pos = reachableHex->r * hexRadius * sqrt(3) + 
                      (reachableHex->q % 2) * hexRadius * sqrt(3) / 2.0;
        
        sf::ConvexShape reachableShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
        reachableShape.setFillColor(sf::Color(100, 255, 100, 80));
        reachableShape.setOutlineColor(sf::Color(112, 129, 88, 255));
        reachableShape.setOutlineThickness(1);
        window_.draw(reachableShape);
    }
}

void renderAttackRange(gl::Ship* selectedShip, const std::vector<gl::Hex>& hexMap) {
    if (!selectedShip) return;
    
    gl::Hex* selectedHex = selectedShip->getCell();
    if (!selectedHex) return;
    
    std::vector<gl::Hex*> attackRangeHexes = selectedShip->cellsInRange(
        *selectedHex, hexMap, selectedShip->getDamageRange(), gl::RangeMode::DAMAGE);
    
    for (gl::Hex* reachableHex : attackRangeHexes) {
        double x_pos = reachableHex->q * hexRadius * 1.5;
        double y_pos = reachableHex->r * hexRadius * sqrt(3) + 
                      (reachableHex->q % 2) * hexRadius * sqrt(3) / 2.0;
        
        sf::ConvexShape reachableShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
        reachableShape.setFillColor(sf::Color(200, 40, 40, 50));
        window_.draw(reachableShape);
    }
}

void renderPath(const std::vector<gl::Hex*>& path) {
    for (auto* hex : path) {
        sf::CircleShape marker(hexRadius / 3);
        marker.setFillColor(sf::Color(100, 200, 255, 120));
        marker.setOrigin(marker.getRadius(), marker.getRadius());
        float hx = hex->q * hexRadius * 1.5f + 50;
        float hy = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0f + 50;
        marker.setPosition(hx, hy);
        window_.draw(marker);
    }
}

void renderSelection(gl::Hex* selectedHex, gl::Ship* selectedShip) {
    if (!selectedShip && selectedHex) {
        double x_pos = selectedHex->q * hexRadius * 1.5;
        double y_pos = selectedHex->r * hexRadius * sqrt(3) + 
                      (selectedHex->q % 2) * hexRadius * sqrt(3) / 2.0;
        
        sf::ConvexShape selectionShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
        selectionShape.setOutlineColor(MAP_COLORS["deep_yellow"]);
        selectionShape.setOutlineThickness(2);
        selectionShape.setFillColor(sf::Color::Transparent);
        window_.draw(selectionShape);
    }
}

void renderShipInfo(const std::vector<std::unique_ptr<gl::Ship>>& ships,
                                const std::vector<gl::Hex*>& vieweableHexes) {
    for (const auto& ship : ships) {
        gl::Hex* cell = ship->getCell();
        if (cell && std::find(vieweableHexes.begin(), vieweableHexes.end(), cell) != vieweableHexes.end()) {
            double x_pos = cell->q * hexRadius * 1.5;
            double y_pos = cell->r * hexRadius * sqrt(3) + (cell->q % 2) * hexRadius * sqrt(3) / 2.0;
            
            if (colScheme_ == COLORS) {
                drawShipBar(window_, ship.get(), x_pos + 50, y_pos + 50, hexRadius, font_, font_size);
            }
        }
    }
}
