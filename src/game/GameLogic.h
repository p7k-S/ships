#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>
#include <string>
#include <variant>
#include <stack>
#include <queue>
#include <unordered_set>

namespace GameLogic {
    class Hex;
    class Ship;

    // Типы игроков
    enum class Owner {
        PLAYER,     // Собственные корабли игрока
        ENEMY,      // Враждебные корабли других государств
        PIRATE,     // Пиратские корабли
        FRIENDLY      // Торговые корабли (нейтральные)
    };

    // Типы клеток
    enum class CellType {
        DEEPWATER,      // Открытое море
        WATER,
        LAND,      // Остров
        FOREST
    };

    // Типы погоды
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

    const std::vector<std::pair<int, int>> DIRECTIONS_EVEN = {
        {1, 0}, {1, -1}, {0, -1}, 
        {-1, -1}, {-1, 0}, {0, 1}
    };

    const std::vector<std::pair<int, int>> DIRECTIONS_ODD = {
        {1, 1}, {1, 0}, {0, -1}, 
        {-1, 0}, {-1, 1}, {0, 1}
    };
}
