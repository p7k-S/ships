#pragma once
#include "../GameLogic.h"
#include "../map/Cell.h"

#include "../entity/Enemy.h"
#include "../entity/Player.h"
#include "../entity/Pirate.h"

#include "../items/BaseItem.h"
#include "BaseTroop.h"

#include <iostream>

namespace GameLogic {

    class Ship : public Troop {
        private:
            Owner owner;
            uint8_t view;         // range(радиус) = 5
            uint8_t move;         // range(радиус) = 3
            uint16_t damage;      // 35
            uint16_t damageRange; // >= 1 (по дефолту 1 далее move - 2) !!не больше чем veiw
            uint16_t health;      // 100
            uint16_t maxHealth;
            uint16_t gold;
            uint16_t maxGold;     // 1000
            std::unique_ptr<Item> item;         //only 1 allowed
            // Hex* curCell;  в базовом классе
        public:
            // Конструктор
            Ship(Owner owner, Hex* currCell):
                owner(owner),
                // curCell(currCell),
                view(255),    // 4  2
                move(4),    // 2  1
                damage(50), // 30 20
                damageRange(3),
                health(100),
                maxHealth(100),
                gold(200),
                maxGold(1000),
                item(nullptr)
        {}


            bool areNeighbors(Hex* h1, Hex* h2);
            
            // Дамаг
            uint16_t getDamageRange() const { return damageRange; }
            void setDamageRange(uint16_t range) { damageRange = range; }
            void takeDamage(uint16_t dmg) {
                if (dmg > health) {
                    health = 0;
                } else {
                    health -= dmg; 
                }
            }
            virtual void giveDamage(Hex* targetCell, std::vector<Hex>& hexMap) {
                Hex* my_cell = getCell();
                if (targetCell == my_cell) return;
                std::vector<Hex*> reachable = cellsInRange(*my_cell, hexMap, my_cell->getTroopOf<Ship>()->getDamageRange(), RangeMode::DAMAGE); // false=не радиус обзора

                if (std::find(reachable.begin(), reachable.end(), targetCell) != reachable.end()) {
                    Ship* enemy = targetCell->getTroopOf<Ship>();
                    enemy->takeDamage(damage);
                    if (enemy->isDestroyed()) {
                        uint16_t enemyGold = enemy->getGold(), tmp = getMaxGold() - getGold();
                        uint16_t getPossibleGold = std::min(enemyGold, tmp);
                        enemy->loseGold(getPossibleGold);
                        takeGold(getPossibleGold);
                        addGoldToCell(targetCell, enemy->getGold());
                    }
                }
            }

            bool isDestroyed() const { return health == 0; }
            void Destroy() {
                Hex* cell = getCell();
                if (cell) {
                    Hex* temp = cell;
                    setCell(nullptr);   // убираем ссылку на клетку
                    temp->removeTroop();  // клетка больше не знает о корабле
                }
            }

            // Хилка
            void heal(uint16_t hp) { hp > maxHealth - health ? health = maxHealth : health += hp; }

            // Голда
            void addGoldToCell(Hex* cell, uint16_t amount) {
                if (!cell) return;
                cell->addStackable<Gold>(amount);
            }
            void takeGoldFromCell(Hex* cell) {
                if (!cell || !cell->hasItemOf<Gold>()) return;
                uint16_t amount = maxGold - gold;
                if (amount) {
                    gold += cell->giveGold(amount);
                }
            }

            void takeGold(uint16_t amount) { amount > maxGold - gold ? gold = maxGold : gold += amount; }
            void loseGold(uint16_t amount) { //amount <= gold ? gold -= amount : gold = 2; }
                if(amount <= gold) {
                    gold -= amount;
                }
            } // сколько надо отдать

            // Методы для предметов
            Item* getItem() const { return item.get(); }

            void takeItemByIndexFromCell(size_t index) {
                if (item) {
                    std::cout << "First remove item from ship\n";
                    return;
                }
                item = getCell()->giveItemByIndex(index);
            }

            std::unique_ptr<Item> loseItem() {
                return std::move(item);
            }

            void giveItemToCell(Hex* cell) {
                if (item && cell) {
                    // Вариант 1: Если addItem должен принимать unique_ptr
                    cell->addItem(std::move(item));
                    // После этого item станет nullptr
                }
            }

            Owner getOwner() const { return owner; }
            void setOwner(Owner newOwner) { owner = newOwner; }
            uint16_t getDamage() const { return damage; }
            uint16_t getHealth() const { return health; }
            uint16_t getMaxHealth() const { return maxHealth; }
            uint16_t getGold() const { return gold; }
            uint16_t getMaxGold() const { return maxGold; }
            uint8_t getView() const { return view; }
            void setView(uint8_t range) { view = range; }

            uint8_t getMoveRange() const { return move; }
            void setMoveRange(uint8_t range) { move = range; }


            // move
            void moveTo(Hex* targetHex, std::vector<Hex>& hexMap) {
                if (!canMoveTo(targetHex, hexMap)) return;
                Hex* my_cell = getCell();
                if (my_cell) my_cell->removeTroop();
                setCell(targetHex);
                targetHex->setTroopOf<Ship>(*this); // Просто передаём ссылку
            }
    };
} //namespace GameLogic
