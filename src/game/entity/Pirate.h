#pragma once
#include "BaseEntity.h"

namespace GameLogic {
    class Pirate : public Entity {
        public:
            Pirate(const std::string& NickName) : Entity(NickName, COLORS["pirate"]) {}
    };
}; // namespace GameLogic
