#include "GameLogic.h"
#include "ship.h"
#include "map.h"
#include <queue>
#include <unordered_map>
#include <vector>

namespace GameLogic {

std::vector<Hex*> Ship::getCellsInRadius(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const bool isView) const
{
    if (!start.getShip()) return {};

    std::vector<Hex*> reachable;

    // --- Быстрый поиск хекса по координатам ---
    auto key = [](int q, int r) -> uint32_t {
        return (static_cast<uint32_t>(q) << 16) | (static_cast<uint32_t>(r) & 0xFFFFu);
    };
    std::unordered_map<uint32_t, Hex*> lookup;
    lookup.reserve(hexMap.size() * 2);
    for (auto& h : hexMap)
        lookup[key(h.q, h.r)] = &h;

    auto getHexAt = [&](int q, int r) -> Hex* {
        auto it = lookup.find(key(q, r));
        return it != lookup.end() ? it->second : nullptr;
    };

    // --- Стоимость перемещения до клетки ---
    std::unordered_map<Hex*, int> moveCost;
    moveCost[&start] = 0;

    std::queue<Hex*> q;
    q.push(&start);
    reachable.push_back(&start);

    while (!q.empty()) {
        Hex* current = q.front();
        q.pop();

        int currentMoveCost = moveCost[current];
        if (currentMoveCost >= maxMoves) continue;

        // Получаем направления в зависимости от четности столбца
        const auto& directions = (current->q % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;

        for (auto [dq, dr] : directions) {
            int nextQ = current->q + dq;
            int nextR = current->r + dr;
            
            Hex* neighbor = getHexAt(nextQ, nextR);
            if (!neighbor) continue; // Клетка за пределами карты
            if (neighbor->isLand() && !isView) continue; // Земля непроходима
            if (neighbor->hasShip() && !isView) continue; // корабль не проходится на сквозь

            // Определяем стоимость перемещения в зависимости от типа клетки
            int terrainCost = (neighbor->getCellType() == CellType::DEEPWATER) ? 2 : 1;
            if (isView) terrainCost = 1;
            int totalCost = currentMoveCost + terrainCost;

            // Если можем добраться до клетки и это новый или более дешевый путь
            if (totalCost <= maxMoves) {
                auto it = moveCost.find(neighbor);
                if (it == moveCost.end() || totalCost < it->second) {
                    moveCost[neighbor] = totalCost;
                    q.push(neighbor);
                    
                    // Добавляем в результат если еще не добавлен
                    if (std::find(reachable.begin(), reachable.end(), neighbor) == reachable.end()) {
                        reachable.push_back(neighbor);
                    }
                }
            }
        }
    }

    return reachable;
}

} // namespace GameLogic
