#pragma once
#include "../map/Cell.h"

namespace GameLogic {
    class Troop {
        Owner owner;
        Hex* curCell;
    public:
        Troop() {}
        Troop(Owner my_owner, Hex* cell) : owner(my_owner), curCell(cell) {}
        virtual ~Troop() = default;  // Добавьте виртуальный деструктор

        // cell
        void setCell(Hex* cell) { curCell = cell; }
        Hex* getCell() const { return curCell; }
        // owner
        Owner getOwner() const { return owner; }
        void setOwner(Owner newOwner) { owner = newOwner; }

        virtual uint8_t getView() const;
        virtual uint8_t getMoveRange() const;
        // Damage
        virtual uint16_t getDamageRange() const;
        virtual void lostResources(Troop* enemy) = 0;
        // virtual void giveDamage(Hex* targetCell, std::vector<Hex>& hexMap);
        virtual void giveDamage(Hex* targetCell) = 0;
        virtual bool isDestroyed() const  = 0;
        virtual void takeDamage(uint16_t dmg) = 0;

        //Gold
        virtual void takeGoldFromCell(Hex* cell) = 0;


        // move
        bool canMoveTo(Hex* targetHex) const; // нгужно ???

        virtual void moveTo(Hex* targetHex);
    };
} // namespace GameLogic
