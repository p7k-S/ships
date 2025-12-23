// #pragma once
#include "BaseTroop.h"
#include "../map/Cell.h"

namespace GameLogic {
    uint8_t Troop::getView() const { 
        return 0; 
    }

    uint8_t Troop::getMoveRange() const { 
        return 0; 
    }

    uint16_t Troop::getDamageRange() const { 
        return 0; 
    }

        bool Troop::canMoveTo(Hex* targetHex) const {
            Hex* my_cell = getCell();
            if (!targetHex || !my_cell) return false;
            if (targetHex == my_cell) return false;
            if (targetHex->hasTroop()) return false;
            if (targetHex->hasBuilding()) {
                if (isEnemy(targetHex->getBuilding()->getOwner(), my_cell->getTroop()->getOwner()))
                    return false;
            }

            return true;
        }

        void Troop::moveTo(Hex* targetHex) {
            if (curCell) {
                curCell = targetHex;
            }
        }
}
