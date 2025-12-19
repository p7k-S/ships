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

        enum class Type { SOLDIER, SHIP };
        virtual Type getType() const = 0;

        // cell
        void setCell(Hex* cell) { curCell = cell; }
        Hex* getCell() const { return curCell; }
        // owner
        Owner getOwner() const { return owner; }
        void setOwner(Owner newOwner) { owner = newOwner; }

        bool isOwnedByCurrentPlayer(Player* currentPlayer) const {
            auto owner = getOwner();
            return std::get_if<Player*>(&owner) && 
                *std::get_if<Player*>(&owner) == currentPlayer;
        }

        virtual void setDamageRange(uint16_t range) = 0;
        virtual void setView(uint8_t range) = 0;
        virtual void setMoveRange(uint8_t range) = 0;
        virtual void setDamage(uint16_t dmg) = 0;
        virtual void setHealth(uint16_t hp) = 0;

        virtual uint16_t getDamage() const = 0;
        virtual uint16_t getHealth() const = 0;
        virtual uint16_t getMaxHealth() const = 0;
        virtual uint16_t getGold() const = 0;
        virtual uint16_t getMaxGold() const = 0;

        virtual void loseGold(uint16_t amount) = 0;

        virtual uint8_t getView() const;
        virtual uint8_t getMoveRange() const;
        // Damage
        virtual uint16_t getDamageRange() const;
        virtual void lostResources(Troop* enemy) = 0;
        // virtual void giveDamage(Hex* targetCell, std::vector<Hex>& hexMap);
        virtual void giveDamageToTroop(Hex* targetCell) = 0;
        virtual void giveDamageToBuilding(Hex* targetCell) = 0;
        virtual bool isDestroyed() const  = 0;
        virtual void takeDamage(uint16_t dmg) = 0;

        virtual void addHP(uint16_t hp) = 0;

        virtual void addItem(std::unique_ptr<Item> newItem) = 0;

        //Gold
        virtual void takeGoldFromCell(Hex* cell) = 0;
        virtual void takeGold(uint16_t amount) = 0;
        //Items
        virtual void takeItemByIndexFromCell(size_t index) = 0;
        virtual bool hasItem() const = 0;

        virtual std::unique_ptr<Item> loseItem() = 0;

        // move
        bool canMoveTo(Hex* targetHex) const; // нгужно ???

        virtual void moveTo(Hex* targetHex);
    };
} // namespace GameLogic
