#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>
#include <string>
#include <variant>
#include <stack>
#include <queue>

namespace GameLogic {
    class Hex;
    // troops type
    class Troop;
    class Ship;
    class Cannon;
    class Soldier;
    class Lighthouse;

    // entity type
    class Player;
    class Enemy;
    class Pirate;

    // items type
    class Item;
    class Gold;
    class Treasure;

    // Building type
    class Building;
    class Port;

    class Entity;

    // weather type
    enum class Weather {
        CALM,       // Штиль
        FOG,        // Туман
        STORM       // Шторм
    };

    enum class SpecialItems {
        ROD,
        MINIONS,
        EAGLE,
        TAILWIND,
        DEVILEYE
    };

    enum class RangeMode {
        MOVE,
        VIEW,
        DAMAGE
    };

    enum class CellType {
        DEEPWATER,
        WATER,
        LAND,
        FOREST
    };

    // Flat top
    const std::vector<std::pair<int, int>> DIRECTIONS_EVEN = {
        {1, 0}, {1, -1}, {0, -1}, 
        {-1, -1}, {-1, 0}, {0, 1}
    };

    const std::vector<std::pair<int, int>> DIRECTIONS_ODD = {
        {1, 1}, {1, 0}, {0, -1}, 
        {-1, 0}, {-1, 1}, {0, 1}
    };

    static std::vector<Hex*> getShortestRoad(std::vector<Hex>& area, Hex* start, Hex* end);
    static std::vector<Hex*> getShortestRoad(std::vector<Hex*>& area, Hex* start, Hex* end);
    bool areNeighbors(Hex* h1, Hex* h2);
    // Hex* getNeighborHex(Hex* hex, int side, const std::vector<Hex*>& allHexes);
    std::vector<Hex> getNeighbors(const Hex& h);
    struct HexEdge;
    struct HexEdgeHash;

    bool portCanPlayced(const Hex& h);
    int hexDistance(const Hex& a, const Hex& b);
    std::vector<Hex*> cellsInRange(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const RangeMode mode); // ship.cpp
    std::vector<HexEdge> getHexEdges(Hex* hex);
    std::vector<HexEdge> getPerimeterEdges(const std::vector<Hex*>& area, const std::vector<Hex*>& allHexes);
    std::vector<Hex*> getBorderHexes(const std::vector<Hex*>& area, const std::vector<Hex*>& allHexes);
    std::vector<Hex*> getBorderHexesWithNeighbors(const std::vector<Hex*>& area, const std::vector<Hex>& allHexes);

    // Owners func
    using Owner = std::variant<Player*, Enemy*, Pirate*>;
    bool isEnemy(Owner p1, Owner p2); // entity/EntityFuncs.cpp
    bool areFriends(Player* p1, Player* p2);
}
