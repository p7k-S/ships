#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <random>
#include "../game/GameLogic.h"
#include "../game/map/PerlinNoise.h"
#include "../game/map/Cell.h"
#include "../game/troops/Ship.h"
#include "../game/GameConfig.h"


namespace gl = GameLogic;

class Game {
public:
    void run();

    uint8_t p_id = playersAmount;
    uint8_t my_pid = 0;
    int8_t move_amount = 0;
    bool isProcessingTurn = true;
    bool changeTurnLocal = false;
private:
    uint16_t totalTurnCount = 0;
    bool endGame = false;
    bool lose = false;

    std::vector<std::unique_ptr<gl::Player>> players;
    gl::Enemy enemy{"Enemy"};
    gl::Pirate pirate{"Pirate"};

    std::vector<gl::Hex> hexMap;

    double deepWater;
    double water;
    double land;

    std::random_device rd;
    std::mt19937 gen;

    gl::Hex* selectedHex = nullptr;
    gl::Troop* selectedTroop = nullptr;
    bool waitingForMove = false;
    bool isDragging = false;
    sf::Vector2f lastMousePos;
    gl::Hex* targetHex = nullptr;

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


    void processEvents();
    void update();
    void render();
    void renderWaitMove();
    void drawCornerDecorations(sf::RenderWindow& window);

    
    bool initialize();
    bool initializeWindow();
    bool initializeResources();
    bool initializeGameWorld();
    bool startMenu();
    bool loadTextures();
    void processPlayerTurn();

    void processBotsTrurns();
    void execPirateAction();
    void execEnemyAction();
    
    void generateMap();
    void distributeCellTypes();
    void createTroops();
    void placingSoldiers(gl::Owner owner);
    bool portCanPlayced(const gl::Hex& h);
    bool placeShip(const gl::Hex& portCell);
    bool placeSoldier(const gl::Hex& portCell);
    void createPlayers();
    void placeGoldAndTreasures();
    
    void handleKeyPressed(const sf::Event& event);
    void handleMouseButtonPressed(const sf::Event& event);
    void handleMouseMove(const sf::Event& event);
    void handleMouseWheel(const sf::Event& event);
    void handleCameraControl(const sf::Event& event);
    void handleWindowResize(const sf::Event& event);

    
    void handleTroopSelection(const sf::Vector2f& worldPos);
    void handleTargetSelection(const sf::Vector2f& worldPos);
    void executeTroopAction();
    bool isPointInHex(const sf::Vector2f& point, const gl::Hex& hex);
    
    uint8_t nextAlivePlayer();
    void nextTurn();
    void troopsOnPortAction();
    void cleanupDestroyedShips();
    void updateVisibleCells();
    void addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Troop* troop, std::vector<gl::Hex>& hexMap, gl::RangeMode mode);
    void addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Building* troop, std::vector<gl::Hex>& hexMap, gl::RangeMode mode);
    
    sf::RenderTexture mapLayer;
    sf::VertexArray shipsBatch;
    sf::Texture shipTextureAtlas;
    bool mapNeedsUpdate = true;
    bool shipsNeedUpdate = true;
    std::vector<sf::Sprite> shipSprites;

    sf::RenderTexture mapSeenLayer;
    std::size_t cachedSeenCount = 0;

    void rebuildSeenMapLayer();
    void renderVisibleCells();
    void renderDynamicObjects();
    sf::Color blendColors(const sf::Color& base, const sf::Color& overlay);


    void rebuildMapLayer();
    void appendHexToVertexArray(sf::VertexArray& va, float cx, float cy, float radius, sf::Color color);
    void rebuildShipBatch();

    void renderMap();
    void renderHex(const gl::Hex& hex, float x_pos, float y_pos);
    void renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite);
    void renderSoldierOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite);
    void renderShipRange();
    void renderRangeHex(gl::Hex* hex, sf::Color fillColor, sf::Color outlineColor);
    void renderUnitBars();
    
    void resetSelection();

    void renderVictoryScreen(uint8_t winnerId);
    void showVictoryScreen(uint8_t winnerId);
    void showLoseScreen();
    void renderLoseScreen();
    void drawVictoryDecorations(sf::RenderWindow& window, uint8_t winnerId);


    void renderSidebar();
    void renderBuildingInfo(gl::Hex* hex, int windowWidth, int& yPos);
    void renderTroopInfo(gl::Troop* troop, int windowWidth, int& yPos);
    void renderBuyUnitsButtons(gl::Troop* troop, int windowWidth, int& yPos);
    void renderTroopUpgrades(gl::Troop* troop, int sidebarX, int sidebarWidth, int& yPos);
    void renderBottomBar();
    void renderUI();

    bool isUIAreaClicked(const sf::Vector2f& mousePos);
    void handleUIClick(const sf::Vector2f& mousePos);

    enum class UpgradeType {
        HEALTH,
        HEAL,
        DAMAGE,
        VIEW_RANGE,
        ATTACK_RANGE,
        MOVE_RANGE,
        BUY_SHIP,
        BUY_SOLDIER,
        BUY_PORT,
        CONVERT
    };

    struct TroopUpgradeButton {
        sf::FloatRect bounds;
        int cost;
        GameLogic::Troop* troop;
        UpgradeType upgradeType;
    };

    std::vector<TroopUpgradeButton> troopUpgradeButtons;

    void handleTroopUpgrade(const TroopUpgradeButton& upgradeButton);



    sf::RenderWindow window;
    sf::View view;
    sf::View defaultView;
    sf::View mapView;
    sf::View uiView;
    bool fullscreenMapMode = false;
    float zoomLevel = 1.f;
    bool isHexInView(float x, float y, float radius, const sf::FloatRect& viewBounds);
    sf::FloatRect getViewBounds();

private:
    bool isPlayerOwner(const gl::Owner& owner) const;
    bool isEnemyOwner(const gl::Owner& owner) const;
    bool isPirateOwner(const gl::Owner& owner) const;

public:
};
