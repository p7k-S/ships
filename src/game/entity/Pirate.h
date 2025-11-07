#pragma once
#include "BaseEntity.h"

namespace GameLogic {
    class Pirate : public Entity {
        public:
            Pirate() : Entity(COLORS["pirate"]) {}
    };
}; // namespace GameLogic
