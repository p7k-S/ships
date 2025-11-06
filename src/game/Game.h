#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
// #include "constants.h"
#include "../game/GameLogic.h"
#include "../game/map/PerlinNoise.h"
#include "../game/map/Cell.h"
#include "../game/troops/Ship.h"
// #include "Renderer.h"
// #include "InputHandler.h"

namespace gl = GameLogic;

class Game {
public:
    Game();
    void run();

private:
        // Основные методы игрового цикла
    void processEvents();
    void update();
    void render();
    
    // Инициализация
    bool initialize();
    bool initializeWindow();
    bool initializeResources();
    bool initializeGameWorld();
    bool startMenu();
    bool loadTextures();
    
    // Генерация игрового мира
    void generateMap();
    void distributeCellTypes();
    void createShips();
    void placeGoldAndTreasures();
    
    // Обработка ввода
    void handleKeyPressed(const sf::Event& event);
    void handleMouseButtonPressed(const sf::Event& event);
    void handleMouseMove(const sf::Event& event);
    void handleMouseWheel(const sf::Event& event);
    void handleCameraControl(const sf::Event& event);
    
    // Логика выбора и действий
    void handleShipSelection(const sf::Vector2f& worldPos);
    void handleTargetSelection(const sf::Vector2f& worldPos);
    void executeShipAction();
    bool isPointInHex(const sf::Vector2f& point, const gl::Hex& hex);
    
    // Обновление состояния игры
    void cleanupDestroyedShips();
    void updateVisibleCells();
    
    // Рендеринг
    void renderMap();
    void renderHex(const gl::Hex& hex, float x_pos, float y_pos);
    void renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite);
    void renderShipRange();
    void renderRangeHex(gl::Hex* hex, sf::Color fillColor, sf::Color outlineColor);
    void renderPath();
    void renderShipUI();

    // Функции для работы с периметром
    // std::vector<gl::Hex*> getBorderHexesWithNeighbors(const std::vector<gl::Hex*>& area, const std::vector<gl::Hex>& allHexes);
    // void renderAttackRangeBorderOnly(const std::vector<gl::Hex*>& attackRangeHexes, const std::vector<gl::Hex>& allHexes);
    
    // Утилиты
    void cleanup();
    void resetSelection();

    std::vector<gl::Hex> hexMap;

    sf::RenderWindow window;
    sf::View view;
    sf::View defaultView;

    // Текстуры
    sf::Texture player_ship_texture;
    sf::Texture pirate_ship_texture;
    sf::Texture enemy_ship_texture;
    sf::Texture gold_texture;
    sf::Texture treasure_texture;

    // // Игровые данные
    std::vector<std::unique_ptr<gl::Ship>> ships;
    std::vector<gl::Hex*> seenCells;
    std::vector<GameLogic::Hex*> vieweableHexes;
    sf::Font font;

    double deepWater;
    double water;
    double land;

    std::random_device rd;
    std::mt19937 gen;

    // Состояние игры
    gl::Hex* selectedHex = nullptr;
    gl::Ship* selectedShip = nullptr;
    bool waitingForMove = false;
    bool isDragging = false;
    sf::Vector2f lastMousePos;
    std::vector<gl::Hex*> currentPath;
    gl::Hex* targetHex = nullptr;

    // Вспомогательные контейнеры
    std::vector<gl::Hex*> deepWaterHexes;
    std::vector<gl::Hex*> waterHexes;
    std::vector<gl::Hex*> landHexes;
    std::vector<gl::Hex*> forestHexes;
};

#endif
