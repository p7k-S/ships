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
    int totalTurnCount = 0;

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
    sf::RenderTexture mapLayer;
    sf::VertexArray shipsBatch;
    sf::Texture shipTextureAtlas;
    bool mapNeedsUpdate = true;
    bool shipsNeedUpdate = true;
    std::vector<sf::Sprite> shipSprites;

    sf::RenderTexture mapSeenLayer;
std::size_t cachedSeenCount = 0;

// Новые методы
void rebuildSeenMapLayer();
void renderVisibleCells();
void renderDynamicObjects();
sf::Color blendColors(const sf::Color& base, const sf::Color& overlay);


    // Методы
    void rebuildMapLayer();
    void appendHexToVertexArray(sf::VertexArray& va, float cx, float cy, float radius, sf::Color color);
    void rebuildShipBatch();

    void renderMap();
    void renderHex(const gl::Hex& hex, float x_pos, float y_pos);
    void renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite);
    void renderShipRange();
    void renderRangeHex(gl::Hex* hex, sf::Color fillColor, sf::Color outlineColor);
    void renderPath();
    void renderShipUI();
    void renderBottomStatsBar();
    void renderCellInfoPanel();

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
    sf::View mapView;
    sf::View uiView;
    bool fullscreenMapMode = false;
    float zoomLevel = 1.f;
    // sf::FloatRect mapViewport = {0.f, 0.f, 0.75f, 0.8f}; // (x, y, width, height) в долях окна sf::FloatRect rightPanelViewport = {0.75f, 0.f, 0.25f, 0.8f};
    // sf::FloatRect bottomBarViewport = {0.f, 0.8f, 1.f, 0.2f};
    bool isHexInView(float x, float y, float radius, const sf::FloatRect& viewBounds);
    sf::FloatRect getViewBounds();

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
