#pragma once
#include "BaseItem.h"
#include <iostream>

namespace GameLogic {
    class Treasure : public Item {
            std::string name;
        public:
            Treasure(std::string str) : name(str) {}
    };
} // namespace GameLogic
