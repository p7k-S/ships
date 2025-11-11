#pragma once
#include "../map/Cell.h"

namespace GameLogic {
    class Troop {
        Hex* curCell;
    public:
        Troop();  // Добавьте конструктор
        virtual ~Troop() = default;  // Добавьте виртуальный деструктор

        void setCell(Hex* cell) { curCell = cell; }
        Hex* getCell() const { return curCell; }

        virtual uint8_t getView() const;
        virtual uint8_t getMoveRange() const;
        virtual uint16_t getDamageRange() const;

        // move
        bool canMoveTo(Hex* targetHex, std::vector<Hex>& hexMap) const; // нгужно ???

        virtual void moveTo(Hex* targetHex, std::vector<Hex>& hexMap);
    };
} // namespace GameLogic
