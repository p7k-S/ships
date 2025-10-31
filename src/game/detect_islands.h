#pragma once
#include <vector>
#include <stack>
#include <unordered_set>

#include "GameLogic.h" // Где объявлен GameLogic::Hex и CellType
// #include "map.h" // Где объявлен GameLogic::Hex и CellType

namespace GameLogic {

struct HexCoord {
    int q, r;
    bool operator==(const HexCoord& o) const { return q == o.q && r == o.r; }
};

struct HexCoordHash {
    std::size_t operator()(const HexCoord& hc) const {
        return std::hash<int>()(hc.q) ^ (std::hash<int>()(hc.r) << 1);
    }
};

using VisitedSet = std::unordered_set<HexCoord, HexCoordHash>;

// Возвращает соседей по hex axial координатам
inline std::vector<HexCoord> getNeighborCoords(int q, int r) {
    static const std::vector<std::pair<int, int>> dirs = {
        {1, 0}, {1,-1}, {0,-1},
        {-1,0}, {-1,1}, {0,1}
    };

    std::vector<HexCoord> res;
    res.reserve(6);
    for (auto& d : dirs) res.push_back({q + d.first, r + d.second});
    return res;
}

inline void dfsIsland(const std::vector<Hex>& map, int width, int height,
                      Hex* start, std::vector<Hex*>& island, VisitedSet& visited) {
    std::stack<Hex*> st;
    st.push(start);
    visited.insert({start->q, start->r});

    while (!st.empty()) {
        Hex* cur = st.top();
        st.pop();
        island.push_back(cur);

        for (auto& [nq, nr] : getNeighborCoords(cur->q, cur->r)) {
            if (nq < 0 || nr < 0 || nq >= width || nr >= height)
                continue;

            Hex* neighbor = const_cast<Hex*>(&map[nr * width + nq]);
            HexCoord nc{neighbor->q, neighbor->r};

            if (neighbor->isLand() && visited.find(nc) == visited.end()) {
                visited.insert(nc);
                st.push(neighbor);
            }
        }
    }
}

inline std::vector<std::vector<Hex*>> findIslands(std::vector<Hex>& map,
                                                  int width, int height) {
    VisitedSet visited;
    std::vector<std::vector<Hex*>> islands;

    for (Hex& hex : map) {
        HexCoord c{hex.q, hex.r};
        if (hex.isLand() && visited.find(c) == visited.end()) {
            std::vector<Hex*> island;
            dfsIsland(map, width, height, &hex, island, visited);
            islands.push_back(island);
        }
    }
    return islands;
}

} // namespace GameLogic
