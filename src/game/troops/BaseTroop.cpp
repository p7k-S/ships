// #pragma once
#include "BaseTroop.h"
#include "../map/Cell.h"

namespace GameLogic {
    // Troop::Troop() : curCell(nullptr) {}

    // Реализация виртуальных методов
    uint8_t Troop::getView() const { 
        return 0; 
    }

    uint8_t Troop::getMoveRange() const { 
        return 0; 
    }

    uint16_t Troop::getDamageRange() const { 
        return 0; 
    }

    // проверяется в отрисовке
        bool Troop::canMoveTo(Hex* targetHex) const { // нгужно ???
            Hex* my_cell = getCell();
            if (!targetHex || !my_cell) return false;
            if (targetHex == my_cell) return false;
            if (targetHex->hasTroop()) return false;
            if (targetHex->hasBuilding()) {
                if (isEnemy(targetHex->getBuilding()->getOwner(), my_cell->getTroop()->getOwner()))
                    return false;
            }

            // std::vector<Hex*> reachable = cellsInRange(*my_cell, hexMap, getMoveRange(), RangeMode::MOVE);
            // for (Hex* cell : reachable)
                // if (cell == targetHex)
                    return true;

            // return false;
        }

        void Troop::moveTo(Hex* targetHex) {
            // Базовая реализация
            if (curCell) {
                curCell = targetHex;
            }
        }
}
