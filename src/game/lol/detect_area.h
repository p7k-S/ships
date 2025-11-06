#include "GameLogic.h"
#include "map.h"

namespace GameLogic {

    inline std::vector<Hex*> getOuterBoundaryHexes(const std::vector<Hex*>& hexes) {
        std::vector<Hex*> outerBoundary;
        std::map<std::pair<int, int>, Hex*> hexMap; // Работает, но медленнее

        // Заполняем карту гексов
        for (Hex* hex : hexes) {
            hexMap[{hex->q, hex->r}] = hex;
        }

        const std::vector<std::pair<int, int>> DIRECTIONS_EVEN = {
            {+1, 0}, {0, -1}, {-1, -1},
            {-1, 0}, {-1, +1}, {0, +1}
        };

        const std::vector<std::pair<int, int>> DIRECTIONS_ODD = {
            {+1, 0}, {+1, -1}, {0, -1},
            {-1, 0}, {0, +1}, {+1, +1}
        };

        // Находим гексы, у которых есть хотя бы один отсутствующий сосед
        for (Hex* hex : hexes) {
            const auto& directions = (hex->r % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;
            bool hasMissingNeighbor = false;

            for (const auto& dir : directions) {
                int nq = hex->q + dir.first;
                int nr = hex->r + dir.second;

                if (hexMap.find({nq, nr}) == hexMap.end()) {
                    hasMissingNeighbor = true;
                    break;
                }
            }

            if (hasMissingNeighbor) {
                outerBoundary.push_back(hex);
            }
        }

        return outerBoundary;
    }

} // namespace gl
