#pragma once
#include "../map/Cell.h"
#include "../troops/Ship.h"

namespace GameLogic {
    class NavigationService {
    public:
        static std::vector<Hex*> findPath(Hex* start, Hex* end, const HexGrid& grid);
        static std::vector<Hex*> getCellsInRange(Hex* start, const HexGrid& grid, 
                                               uint8_t range, RangeMode mode);

        bool canMoveTo(Hex* targetHex, std::vector<Hex>& hexMap) const {
            if (!targetHex || !curCell) return false;
            if (targetHex == curCell) return false;
            if (targetHex->isLand() || targetHex->hasShip()) return false;

            std::vector<Hex*> reachable = cellsInRange(*curCell, hexMap, curCell->getShip()->getMoveRange(), RangeMode::MOVE);
            for (Hex* cell : reachable)
                if (cell == targetHex)
                    return true;

            return false;
        }

        static bool canMoveTo(const Ship& ship, Hex* target, const HexGrid& grid);
        static int calculateDistance(const Hex& a, const Hex& b);
    };
}

