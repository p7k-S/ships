#pragma once
#include "../GameLogic.h"

namespace GameLogic {
    class Building {
            Owner owner;
            Hex* curCell;
        public:
            Building(Owner my_owner, Hex* cell) : owner(my_owner), curCell(cell) {}

            enum class Type { PORT };
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


            virtual uint8_t getView() const;
            // Damage
            virtual uint16_t getDamageRange() const;
            virtual void lostResources(Troop* enemy) = 0;
            virtual uint16_t getHealth() const = 0;
            virtual uint16_t getHeal() const = 0;
            virtual uint16_t getMaxHealth() const = 0;
            virtual uint16_t getGold() const = 0;

            virtual bool isDestroyed() const  = 0;
            virtual void takeDamage(uint16_t dmg) = 0;

            virtual bool isPort() const { return false; }
    };
}
