#include <SFML/Graphics.hpp>
#include "../GameLogic.h"
#include "../map/Cell.h"
// #include "../troops/Ship.h"
// #include <queue>
#include <climits>
#include <unordered_map>
#include <vector>

namespace GameLogic {

bool areNeighbors(const Hex& h1, const Hex& h2) {
    int dq = abs(h1.q - h2.q);
    int dr = abs(h1.r - h2.r);
    
    return (dq == 1 && dr == 0) || 
           (dq == 0 && dr == 1) ||
           (dq == 1 && dr == 1 && (h1.q % 2 == h2.q % 2));  // оба четные или оба нечетные
}

std::vector<Hex> getNeighbors(const Hex& h) {
    const auto& dirs = (h.r % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;
    std::vector<Hex> result;
    for (auto [dq, dr] : dirs) {
        result.push_back({h.q + dq, h.r + dr});
    }
    return result;
}

// mode = attack/view/move
std::vector<Hex*> cellsInRange(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const RangeMode mode)
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
            if (neighbor->isLand() && mode != RangeMode::VIEW) continue; // Земля непроходима
            if (neighbor->hasShip() && mode == RangeMode::MOVE) continue; // корабль не проходится на сквозь

            // Определяем стоимость перемещения в зависимости от типа клетки
            int terrainCost = (neighbor->getCellType() == CellType::DEEPWATER) ? 2 : 1;
            if (mode == RangeMode::VIEW) terrainCost = 1;
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


// Хэш-функция для Hex
struct HexHash {
    std::size_t operator()(const Hex& h) const {
        return std::hash<int>()(h.q) ^ (std::hash<int>()(h.r) << 1);
    }
};

// Узел для приоритетной очереди
struct Node {
    Hex* hex;
    int f_cost; // f = g + h
    
    bool operator>(const Node& other) const {
        return f_cost > other.f_cost;
    }
};

int heuristic(const Hex& a, const Hex& b) {
    // Манхэттенское расстояние для шестиугольных координат
    return (std::abs(a.q - b.q) + std::abs(a.r - b.r) + std::abs(-a.q - a.r + b.q + b.r)) / 2;
}

std::vector<Hex*> findRoad(Hex& start, Hex& end, std::vector<Hex>& allowedArea) {
    // Создаем карту быстрого доступа к разрешенным гексам
    std::unordered_map<Hex, Hex*, HexHash> allowedMap;
    std::vector<Hex*> allowedPtrs;
    
    for (auto& hex : allowedArea) {
        allowedMap[hex] = &hex;
        allowedPtrs.push_back(&hex);
    }
    
    // Если старт или финиш не в разрешенной зоне - путь невозможен
    if (allowedMap.find(start) == allowedMap.end() || 
        allowedMap.find(end) == allowedMap.end()) {
        return {};
    }
    
    // Структуры для алгоритма A*
    std::unordered_map<Hex, Hex*, HexHash> cameFrom;
    std::unordered_map<Hex, int, HexHash> gScore;
    std::unordered_map<Hex, int, HexHash> fScore;
    
    // Инициализация
    for (auto& hex : allowedArea) {
        gScore[hex] = INT_MAX;
        fScore[hex] = INT_MAX;
    }
    gScore[start] = 0;
    fScore[start] = heuristic(start, end);
    
    // Приоритетная очередь для открытых узлов
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    openSet.push({&start, fScore[start]});
    
    while (!openSet.empty()) {
        Hex* current = openSet.top().hex;
        openSet.pop();
        
        // Если дошли до цели
        if (*current == end) {
            // Восстанавливаем путь
            std::vector<Hex*> path;
            Hex* pathNode = current;
            
            while (pathNode != nullptr) {
                path.push_back(pathNode);
                auto it = cameFrom.find(*pathNode);
                pathNode = (it != cameFrom.end()) ? it->second : nullptr;
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Получаем соседей текущего гекса
        std::vector<Hex*> neighbors;
        std::vector<Hex> potentialNeighbors = getNeighbors(*current);
        
        for (auto& neighbor : potentialNeighbors) {
            auto it = allowedMap.find(neighbor);
            if (it != allowedMap.end()) {
                neighbors.push_back(it->second);
            }
        }
        
        // Обрабатываем каждого соседа
        for (Hex* neighbor : neighbors) {
            int tentative_gScore = gScore[*current] + 1; // стоимость перехода всегда 1
            
            if (tentative_gScore < gScore[*neighbor]) {
                // Этот путь лучше предыдущего
                cameFrom[*neighbor] = current;
                gScore[*neighbor] = tentative_gScore;
                fScore[*neighbor] = tentative_gScore + heuristic(*neighbor, end);
                openSet.push({neighbor, fScore[*neighbor]});
            }
        }
    }
    
    // Путь не найден
    return {};
}

/*
std::vector<Hex*> Ship::getShortestRoad(std::vector<Hex>& area, Hex* start, Hex* end) {
    std::queue<Hex*> frontier;
    frontier.push(start);

    std::map<std::pair<int,int>, std::pair<int,int>> came_from;
    came_from[{start->q, start->r}] = {-9999, -9999};

    while (!frontier.empty()) {
        Hex* current = frontier.front();
        frontier.pop();

        if (current == end) break;

        const auto& dirs = (current->r % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;
        for (auto [dq, dr] : dirs) {
            int nq = current->q + dq;
            int nr = current->r + dr;
            for (auto& h : area) {
                if (h.q == nq && h.r == nr) {
                    if (!came_from.count({nq, nr})) {
                        came_from[{nq, nr}] = {current->q, current->r};
                        frontier.push(&h);
                    }
                }
            }
        }
    }

    std::vector<Hex*> path;
    std::pair<int,int> cur = {end->q, end->r};

    if (!came_from.count(cur))
        return path; // пустой — нет пути

    while (cur != std::pair<int,int>{-9999, -9999}) {
        for (auto& h : area)
            if (h.q == cur.first && h.r == cur.second)
                path.push_back(&h);
        cur = came_from[cur];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Hex*> Ship::getShortestRoad(std::vector<Hex*>& area, Hex* start, Hex* end) {
    // новая реализация для вектора указателей
    std::queue<Hex*> frontier;
    frontier.push(start);

    std::map<std::pair<int,int>, std::pair<int,int>> came_from;
    came_from[{start->q, start->r}] = {-9999, -9999};

    while (!frontier.empty()) {
        Hex* current = frontier.front();
        frontier.pop();

        if (current == end) break;

        const auto& dirs = (current->r % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;
        for (auto [dq, dr] : dirs) {
            int nq = current->q + dq;
            int nr = current->r + dr;
            
            for (auto* h : area) {
                if (h->q == nq && h->r == nr) {
                    if (!came_from.count({nq, nr})) {
                        came_from[{nq, nr}] = {current->q, current->r};
                        frontier.push(h);
                    }
                }
            }
        }
    }

    std::vector<Hex*> path;
    std::pair<int,int> cur = {end->q, end->r};

    if (!came_from.count(cur))
        return path;

    while (cur != std::pair<int,int>{-9999, -9999}) {
        for (auto* h : area) {
            if (h->q == cur.first && h->r == cur.second) {
                path.push_back(h);
                break; // можно добавить break так как координаты уникальны
            }
        }
        cur = came_from[cur];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

*/

} // namespace GameLogic

