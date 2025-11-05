#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "../game/GameLogic.h"
namespace gl = GameLogic;

class Renderer {
public:
    static void renderGame(sf::RenderWindow& window,
                          sf::View& view,
                          std::vector<gl::Hex>& hexMap,
                          std::vector<std::unique_ptr<gl::Ship>>& ships,
                          std::vector<gl::Hex*>& seenCells,
                          gl::Hex* selectedHex,
                          gl::Ship* selectedShip,
                          bool waitingForMove,
                          gl::Hex* targetHex,
                          std::vector<gl::Hex*>& currentPath,
                          sf::Texture& player_ship_texture,
                          sf::Texture& pirate_ship_texture,
                          sf::Texture& enemy_ship_texture,
                          sf::Texture& gold_texture,
                          sf::Texture& treasure_texture);
    
private:
    static void renderHexMap(sf::RenderWindow& window,
                            std::vector<gl::Hex>& hexMap,
                            std::vector<gl::Hex*>& seenCells,
                            std::vector<std::unique_ptr<gl::Ship>>& ships,
                            gl::Hex* selectedHex,
                            sf::Texture& player_ship_texture,
                            sf::Texture& pirate_ship_texture,
                            sf::Texture& enemy_ship_texture,
                            sf::Texture& gold_texture,
                            sf::Texture& treasure_texture);
    
    static void renderShipRange(sf::RenderWindow& window,
                               gl::Ship* selectedShip,
                               std::vector<gl::Hex>& hexMap);
    
    static void renderPath(sf::RenderWindow& window,
                          std::vector<gl::Hex*>& currentPath);
    
    static void renderShipBars(sf::RenderWindow& window,
                              std::vector<std::unique_ptr<gl::Ship>>& ships,
                              std::vector<gl::Hex>& hexMap,
                              std::vector<gl::Hex*>& vieweableHexes);
};

#endif
