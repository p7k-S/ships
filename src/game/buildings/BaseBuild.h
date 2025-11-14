#pragma once
#include "../GameLogic.h"

namespace GameLogic {
    class Building {
            Owner owner;
            Hex* curCell;
        public:
            Building(Owner my_owner, Hex* cell) : owner(my_owner), curCell(cell) {}
    };
}
