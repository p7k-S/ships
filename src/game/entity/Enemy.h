#pragma once
#include "BaseEntity.h"

namespace GameLogic {
    class Enemy : public Entity {
        public:
            Enemy(const std::string& NickName) : Entity(NickName, COLORS["enemy"]) {}
    };
}; // namespace GameLogic
