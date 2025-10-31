#include "GameLogic.h"
#include "ship.h"
#include "map.h"
#include <climits>
#include <queue>
#include <unordered_map>
#include <vector>

namespace GameLogic {

std::vector<Hex*> Ship::getCellsInRadius(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const bool isView) const
{
    if (!start.getShip()) return {};

    std::vector<Hex*> reachable;

    // --- Определяем границы карты ---
    int minQ = INT_MAX, maxQ = INT_MIN;
    int minR = INT_MAX, maxR = INT_MIN;
    
    for (const auto& h : hexMap) {
        if (h.q < minQ) minQ = h.q;
        if (h.q > maxQ) maxQ = h.q;
        if (h.r < minR) minR = h.r;
        if (h.r > maxR) maxR = h.r;
    }
    
    int widthQ = maxQ - minQ + 1;
    int heightR = maxR - minR + 1;

    // --- Быстрый поиск хекса по координатам ---
    auto key = [](int q, int r) -> uint32_t {
        return (static_cast<uint32_t>(q) << 16) | (static_cast<uint32_t>(r) & 0xFFFFu);
    };
    std::unordered_map<uint32_t, Hex*> lookup;
    lookup.reserve(hexMap.size() * 2);
    for (auto& h : hexMap)
        lookup[key(h.q, h.r)] = &h;

    // Функция для получения хекса с учетом цикличности
    auto getHexAt = [&](int q, int r) -> Hex* {
        // "Заворачиваем" координаты по тору
        int wrappedQ = minQ + ((q - minQ) % widthQ + widthQ) % widthQ;
        int wrappedR = minR + ((r - minR) % heightR + heightR) % heightR;
        
        auto it = lookup.find(key(wrappedQ, wrappedR));
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
        // ВАЖНО: Для цикличной карты нужно учитывать, что при переходе через границу
        // четность столбца может измениться!
        bool isCurrentEven = (current->q % 2 == 0);
        const auto& directions = isCurrentEven ? DIRECTIONS_EVEN : DIRECTIONS_ODD;

        for (auto [dq, dr] : directions) {
            int nextQ = current->q + dq;
            int nextR = current->r + dr;
            
            // Вычисляем "завернутые" координаты
            int wrappedQ = minQ + ((nextQ - minQ) % widthQ + widthQ) % widthQ;
            int wrappedR = minR + ((nextR - minR) % heightR + heightR) % heightR;
            
            Hex* neighbor = getHexAt(wrappedQ, wrappedR);
            if (!neighbor) continue;
            
            // ПРОВЕРКА: Убедимся, что это действительно сосед с учетом цикличности
            // Для этого проверим, что разница координат соответствует направлению
            int deltaQ = wrappedQ - current->q;
            int deltaR = wrappedR - current->r;
            
            // Если переход через границу, корректируем разницу
            if (abs(deltaQ) > widthQ / 2) {
                deltaQ = (deltaQ > 0) ? deltaQ - widthQ : deltaQ + widthQ;
            }
            if (abs(deltaR) > heightR / 2) {
                deltaR = (deltaR > 0) ? deltaR - heightR : deltaR + heightR;
            }
            
            // Проверяем, соответствует ли разница ожидаемому направлению
            bool isValidNeighbor = false;
            for (auto [expDQ, expDR] : directions) {
                if (deltaQ == expDQ && deltaR == expDR) {
                    isValidNeighbor = true;
                    break;
                }
            }
            
            if (!isValidNeighbor) continue;
            
            if (neighbor->isLand() && !isView) continue;
            if (neighbor->hasShip() && !isView) continue;

            int terrainCost = (neighbor->getCellType() == CellType::DEEPWATER) ? 2 : 1;
            if (isView) terrainCost = 1;
            int totalCost = currentMoveCost + terrainCost;

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
