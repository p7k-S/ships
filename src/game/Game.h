#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <random>
// #include "constants.h"
#include "../game/GameLogic.h"
#include "../game/map/PerlinNoise.h"
#include "../game/map/Cell.h"
#include "../game/troops/Ship.h"
#include "../game/GameConfig.h"
// #include "Renderer.h"
// #include "InputHandler.h"
#include "../net/Server.h"
#include "../net/Client.h"


namespace gl = GameLogic;

class Game {
public:
    Game();
    void run();

    uint8_t p_id = playersAmount;
    uint8_t my_pid = 0;
    int8_t move_amount = 0;
    bool isProcessingTurn = true;
    // net
    std::unique_ptr<GameServer> gameServer;
    std::unique_ptr<GameClient> gameClient;
    bool isHost = false;
    uint8_t connectedPlayers = 0;
    bool isNetworkGame = false; // ⬅️ ДОБАВИТЬ ЭТО

    bool changeTurnLocal = false;
private:
    // net
    void handleNetworkMessages();
    void sendNetworkMessage(const std::string& message);
    void createLocalPlayer();
    void processServerMessage(const std::string& msg);
    void processClientMessage(const std::string& msg);
    void sendMap();
    void receiveMap(const std::string& msg);
    void sendPlayers();
    void receivePlayers(const std::string& msg);
    
    void sendTroopAction(int fromQ, int fromR, int toQ, int toR);
    void executeNetworkAction(const std::string& msg);

    int8_t getMoveAmount() const {
        return move_amount;
    }
    void setMoveAmount(int8_t moves) {
        move_amount = moves;
    }
    void sendGameStateUpdate();
    void sendShipCreation(int playerIndex, int shipQ, int shipR, const std::string& shipType);
    void sendTurnUpdate();
    void handleMoveAction(int playerId, int fromQ, int fromR, int toQ, int toR);
    void handleAttackAction(int playerId, int attackerQ, int attackerR, int targetQ, int targetR);

    uint16_t totalTurnCount = 0;

    // Основные методы игрового цикла
    void processEvents();
    void update();
    void render();
    void renderWaitMove();
    
    // Инициализация
    bool initialize();
    bool initializeWindow();
    bool initializeResources();
    bool initializeGameWorld();
    bool startMenu();
    bool loadTextures();
    void processPlayerTurn();
    
    // Генерация игрового мира
    void generateMap();
    void distributeCellTypes();
    void createTroops();
    void placingSoldiers(gl::Owner owner);
    bool portCanPlayced(const gl::Hex& h);
    void createPlayers();
    void placeGoldAndTreasures();
    
    // Обработка ввода
    void handleKeyPressed(const sf::Event& event);
    void handleMouseButtonPressed(const sf::Event& event);
    void handleMouseMove(const sf::Event& event);
    void handleMouseWheel(const sf::Event& event);
    void handleCameraControl(const sf::Event& event);
    
    // Логика выбора и действий
    void handleTroopSelection(const sf::Vector2f& worldPos);
    void handleTargetSelection(const sf::Vector2f& worldPos);
    void executeTroopAction();
    bool isPointInHex(const sf::Vector2f& point, const gl::Hex& hex);
    
    // Обновление состояния игры
    uint8_t nextAlivePlayer();
    void nextTurn();
    void cleanupDestroyedShips();
    void updateVisibleCells();
    void addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Troop* troop, std::vector<gl::Hex>& hexMap, gl::RangeMode mode);
    
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
    void renderBars();
    void renderBottomStatsBar();
    void renderCellInfoPanel();

    // Функции для работы с периметром
    // std::vector<gl::Hex*> getBorderHexesWithNeighbors(const std::vector<gl::Hex*>& area, const std::vector<gl::Hex>& allHexes);
    // void renderAttackRangeBorderOnly(const std::vector<gl::Hex*>& attackRangeHexes, const std::vector<gl::Hex>& allHexes);
    
    // Утилиты
    void cleanup();
    void resetSelection();

    std::vector<gl::Hex> hexMap;


    void renderSidebar();
    void renderBottomBar();
    void renderUI(); // основной метод для всего UI

    // Методы для обработки UI кликов
    bool isUIAreaClicked(const sf::Vector2f& mousePos);
    void handleUIClick(const sf::Vector2f& mousePos);

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
    // sf::Texture player_ship_texture;
    // sf::Texture pirate_ship_texture;
    // sf::Texture enemy_ship_texture;
    // sf::Texture gold_texture;
    // sf::Texture treasure_texture;


    // Все игроки и игровые сущности
    std::vector<std::unique_ptr<gl::Player>> players;
    gl::Enemy enemy{"Enemy"};
    gl::Pirate pirate{"Pirate"};


    // это for надо по хорошему на старте как поставят
    // std::vector<gl::Ship*> ships;
    // std::vector<gl::Hex*> seenCells;
    // std::vector<gl::Hex*> viewableHexes;
    // sf::Font font;

    double deepWater;
    double water;
    double land;

    std::random_device rd;
    std::mt19937 gen;

    // Состояние игры
    gl::Hex* selectedHex = nullptr;
    gl::Troop* selectedTroop = nullptr;
    bool waitingForMove = false;
    bool isDragging = false;
    sf::Vector2f lastMousePos;
    std::vector<gl::Hex*> currentPath;
    gl::Hex* targetHex = nullptr;

    // Вспомогательные контейнеры
    // std::vector<gl::Hex*> deepWaterHexes;
    // std::vector<gl::Hex*> waterHexes;
    // std::vector<gl::Hex*> landHexes;
    // std::vector<gl::Hex*> forestHexes;

private:
    bool isPlayerOwner(const gl::Owner& owner) const;
    bool isEnemyOwner(const gl::Owner& owner) const;
    bool isPirateOwner(const gl::Owner& owner) const;

public:
    // gl::Player* getPlayer() const { 
    //     return players[0].get();  // .get() для unique_ptr
    // }
    // gl::Enemy* getEnemy() { 
    //     return &enemy;  // Берем адрес объекта
    // }
    //
    // gl::Pirate* getPirate() { 
    //     return &pirate; // Берем адрес объекта
    // }
};
