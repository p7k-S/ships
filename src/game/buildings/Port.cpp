// Port.cpp
#include "Port.h"
#include "../map/Cell.h"

namespace GameLogic {

void Port::lostResources(Troop* enemy) {
    Hex* cell = getCell();

    if (!enemy->hasItem() && !items.empty()) {
        enemy->addItem(std::move(items.back()));
        items.pop_back();
    }

    while (!items.empty()) {
        if (cell) {
            cell->addItem(std::move(items.back()));
        }
        items.pop_back();
    }
}

} // namespace GameLogic

