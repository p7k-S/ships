#pragma once
#include "BaseEntity.h"

namespace GameLogic {
    class Enemy : public Entity {
        public:
            Enemy() : Entity(COLORS["enemy"]) {}
    };
}; // namespace GameLogic
