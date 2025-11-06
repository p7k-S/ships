#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../game/GameLogic.h"
#include "../game/map/Cell.h"
#include "../game/troops/Ship.h"
#include "Colors.hpp"
#include "info_bars.h"
#include "sprites.h"

class GameRenderer {
private:
    sf::RenderWindow& window_;
    sf::Font& font_;
    
    // Текстуры
    sf::Texture player_ship_texture_;
    sf::Texture pirate_ship_texture_;
    sf::Texture enemy_ship_texture_;
    sf::Texture gold_texture_;
    sf::Texture treasure_texture_;
    
    // Настройки отображения
    ColorScheme colScheme_;
    ColorScheme colSchemeInactive_;
    double deepWater_;
    double water_;
    double land_;
    
public:
    GameRenderer(sf::RenderWindow& window, sf::Font& font);
    
    bool loadTextures();
    void setColorSchemes(ColorScheme active, ColorScheme inactive);
    void setTerrainThresholds(double deepWater, double water, double land);
    
    // Основные методы рендеринга
    void renderMap(const std::vector<gl::Hex*>& seenCells, 
                   const std::vector<gl::Hex*>& vieweableHexes,
                   const std::vector<gl::Hex>& hexMap);
    
    void renderMovementRange(gl::Ship* selectedShip, 
                           const std::vector<gl::Hex>& hexMap);
    
    void renderAttackRange(gl::Ship* selectedShip, 
                          const std::vector<gl::Hex>& hexMap);
    
    void renderPath(const std::vector<gl::Hex*>& path);
    void renderSelection(gl::Hex* selectedHex, gl::Ship* selectedShip);
    void renderShipInfo(const std::vector<std::unique_ptr<gl::Ship>>& ships,
                       const std::vector<gl::Hex*>& vieweableHexes);
    
private:
    // Вспомогательные методы
    void renderHex(const gl::Hex& hex, float x_pos, float y_pos, bool isVisible);
    void renderShipOnHex(const gl::Hex& hex, float x_pos, float y_pos);
    void renderResourcesOnHex(const gl::Hex& hex, float x_pos, float y_pos);
    void renderHexOutline(const gl::Hex& hex, sf::ConvexShape& hexShape);
    
    sf::Color getHexColor(const gl::Hex& hex, bool isVisible) const;
};
