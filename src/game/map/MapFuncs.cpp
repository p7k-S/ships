#include <SFML/Graphics.hpp>
#include "../GameLogic.h"
#include "../map/Cell.h"
#include "../troops/Ship.h"
#include "../buildings/Port.h"
#include "../troops/Soldier.h"
#include <climits>
#include <unordered_map>
#include <vector>

namespace GameLogic {

    bool areNeighbors(const Hex& h1, const Hex& h2) {
        int dq = abs(h1.q - h2.q);
        int dr = abs(h1.r - h2.r);

        return (dq == 1 && dr == 0) || 
            (dq == 0 && dr == 1) ||
            (dq == 1 && dr == 1 && (h1.q % 2 == h2.q % 2));
    }

    int hexDistance(const Hex& a, const Hex& b) {
        return (abs(a.q - b.q) + abs(a.q + a.r - b.q - b.r) + abs(a.r - b.r)) / 2;
    }


    std::vector<Hex> getNeighbors(const Hex& h) {
        const auto& dirs = (h.q % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;

        std::vector<Hex> result;
        for (auto [dq, dr] : dirs) {
            result.push_back({h.q + dq, h.r + dr});
        }
        return result;
    }

    bool portCanPlayced(const Hex& h) {
        if (h.getCellType() != CellType::WATER) {
            return false;
        }
        return false;
    }

    // mode = attack/view/move
std::vector<Hex*> cellsInRange(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const RangeMode mode)
{
    if (!(start.hasTroop() || start.hasBuilding())) return {};

    std::vector<Hex*> reachable;

    Troop* startTroop = start.getTroop();
    bool isShip = false;
    bool isSoldier = false;
    
    if (startTroop) {
        if (typeid(*startTroop) == typeid(Ship)) {
            isShip = true;
        } else if (typeid(*startTroop) == typeid(Soldier)) {
            isSoldier = true;
        }
    }

    // Быстрый поиск хекса по координатам
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

    auto hasPort = [](Hex* hex) -> bool {
        if (!hex || !hex->hasBuilding()) return false;
        Building* building = hex->getBuilding();
        return typeid(*building) == typeid(Port);
    };

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
            
            // проверки проходимости
            if (mode != RangeMode::VIEW) {
                if (isShip) {
                    if (!neighbor->isWater()) continue;
                } else if (isSoldier) {
                    if (!neighbor->isLand() && !(neighbor->isWater() && hasPort(neighbor))) {
                        continue;
                    }
                } else {
                    // Для других типов или если troop не определен
                    if (neighbor->isLand()) continue;
                }
            }
            
            if (neighbor->hasTroop() && mode == RangeMode::MOVE) continue; // корабль не проходится на сквозь

            // определение стоимости перемещения
            int terrainCost = 1;
            if (mode != RangeMode::VIEW) {
                if (isShip) {
                    terrainCost = (neighbor->getCellType() == CellType::DEEPWATER) ? 2 : 1;
                } else if (isSoldier) {
                    if (neighbor->isLand()) {
                        terrainCost = (neighbor->getCellType() == CellType::FOREST) ? 2 : 1;
                    } else {
                        terrainCost = 1;
                    }
                } else {
                    // Для других типов сохраняем старую логику
                    terrainCost = (neighbor->getCellType() == CellType::DEEPWATER) ? 2 : 1;
                }
            }

            int totalCost = currentMoveCost + terrainCost;

            // Если можем добраться до клетки и это новый или более дешевый путь
            if (totalCost <= maxMoves) {
                auto it = moveCost.find(neighbor);
                if (it == moveCost.end() || totalCost < it->second) {
                    moveCost[neighbor] = totalCost;
                    q.push(neighbor);

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

