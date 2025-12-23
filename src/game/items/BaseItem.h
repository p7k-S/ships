#pragma once

#include <cstdint>
#include <memory>

namespace GameLogic {
    class Item {
    public:
        virtual bool isStackable() const { return false; }
        virtual uint16_t getItemAmount() const { return 0; }
        virtual void addItemAmount(uint16_t adding_amount) {}
        virtual void setItemAmount(uint16_t new_amount) {}

        virtual ~Item() = default;
    };
}; // namespace GameLogic
