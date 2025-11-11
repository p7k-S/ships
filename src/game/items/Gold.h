#pragma once
#include "BaseItem.h"
#include <cstdint>

namespace GameLogic {
    class Gold : public Item {
        uint16_t gold;

    public:
        Gold(uint16_t amount) : gold(amount) {} 

        bool isStackable() const override { return true; }

        uint16_t getItemAmount() const override {
            return gold;
        }
        void addItemAmount(uint16_t adding_amount) override {
            gold += adding_amount;
        }
        void setItemAmount(uint16_t new_amount) override {
            gold = new_amount;
        }
    };
} // namespace GameLogic
