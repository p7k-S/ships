#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../game/constants.h"
#include "../game/GameLogic.h"

namespace gl = GameLogic;

class Game {
private:
    sf::RenderWindow window;
    sf::View view;
    sf::View defaultView;
    
    // Состояние игры
    gl::Hex* selectedHex = nullptr;
    gl::Ship* selectedShip = nullptr;
    bool waitingForMove = false;
    bool isDragging = false;
    sf::Vector2f lastMousePos;
    
    std::vector<gl::Hex*> currentPath;
    gl::Hex* targetHex = nullptr;
    
    // Текстуры
    sf::Texture player_ship_texture;
    sf::Texture pirate_ship_texture;
    sf::Texture enemy_ship_texture;
    sf::Texture gold_texture;
    sf::Texture treasure_texture;
    
    // Игровые данные (передаются извне)
    std::vector<gl::Hex>& hexMap;
    std::vector<std::unique_ptr<gl::Ship>>& ships;
    std::vector<gl::Hex*>& seenCells;
    
public:
    Game(std::vector<gl::Hex>& hexMap, 
         std::vector<std::unique_ptr<gl::Ship>>& ships,
         std::vector<gl::Hex*>& seenCells);
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    bool loadTextures();
    void handleEvents();
    void update();
    void render();
    
    void handleCameraControls(sf::Event& event);
    void handleMouseClick(sf::Vector2f worldPos);
    void executeShipAction();
    void cleanupDestroyedShips();
};

#endif
