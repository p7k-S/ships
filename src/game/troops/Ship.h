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
            uint8_t view = 5;         // range(радиус) = 5
            uint8_t move = 4;         // range(радиус) = 3
            uint16_t damage = 50;      // 35
            uint16_t damageRange = 3; // >= 1 (по дефолту 1 далее move - 2) !!не больше чем view
            uint16_t health = 100;      // 100
            uint16_t maxHealth = 100;
            uint16_t gold = 0;
            uint16_t maxGold = 1000;     // 1000
            std::unique_ptr<Item> item = nullptr;         //only 1 allowed
            /*   в BaseTroop
            Owner owner;
            Hex* curCell;
            */
        public:
            // Конструктор
            Ship(Owner owner, Hex* currCell):
                Troop(owner, currCell)
                // view(5),    // 4  2
                // move(4),    // 2  1
                // damage(50), // 30 20
                // damageRange(3),
                // health(100),
                // maxHealth(100),
                // gold(200),
                // maxGold(1000),
                // item(nullptr)
        {}


            // bool areNeighbors(Hex* h1, Hex* h2);
            
            // Дамаг
            uint16_t getDamageRange() const override { return damageRange; }
            void setDamageRange(uint16_t range) { damageRange = range; }
            virtual void takeDamage(uint16_t dmg) override { dmg > health ? health = 0 : health -= dmg; }
            virtual void giveDamage(Hex* targetCell) override {
                // Hex* my_cell = getCell();
                // if (targetCell == my_cell) return;
                // std::vector<Hex*> reachable = cellsInRange(*my_cell, hexMap, my_cell->getTroop()->getDamageRange(), RangeMode::DAMAGE); // false=не радиус обзора

                // if (std::find(reachable.begin(), reachable.end(), targetCell) != reachable.end()) {
                    Troop* enemy = targetCell->getTroop();
                    enemy->takeDamage(damage);
                    if (enemy->isDestroyed()) {
                        lostResources(enemy);
                        targetCell->removeTroop();


                        std::visit(
                            [enemy](auto* ownerPtr) {
                                if (ownerPtr) {
                                    ownerPtr->removeTroop(enemy);
                                }
                            }, 
                            enemy->getOwner()
                        );
                    }
                // }
            }

            virtual bool isDestroyed() const override { return health == 0; }
            virtual void lostResources(Troop* enemy) override {
                Ship* enemy_ship = static_cast<Ship*>(enemy);

                if (enemy_ship->isDestroyed()) {
                    uint16_t enemyGold = enemy_ship->getGold();
                    uint16_t availableSpace = getMaxGold() - getGold();
                    uint16_t transferableGold = std::min(enemyGold, availableSpace);

                    // Забираем сколько влезает
                    enemy_ship->loseGold(transferableGold);
                    takeGold(transferableGold);

                    // Остальное высыпаем на клетку
                    uint16_t remainingGold = enemy_ship->getGold();
                    Hex* targetCell = enemy_ship->getCell();
                    if (remainingGold > 0) {
                        addGoldToCell(targetCell, remainingGold);
                        enemy_ship->loseGold(remainingGold);
                    }
                }
            }

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
                cell->addGold(amount);
            }
            virtual void takeGoldFromCell(Hex* cell) override {
                if (!cell) return;
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

            uint16_t getDamage() const { return damage; }
            uint16_t getHealth() const { return health; }
            uint16_t getMaxHealth() const { return maxHealth; }
            uint16_t getGold() const { return gold; }
            uint16_t getMaxGold() const { return maxGold; }
            virtual uint8_t getView() const override { return view; }
            void setView(uint8_t range) { view = range; }

            virtual uint8_t getMoveRange() const override { return move; }
            void setMoveRange(uint8_t range) { move = range; }


            // move
            // В классе Ship
            virtual void moveTo(Hex* targetHex) override {
                if (!targetHex || !canMoveTo(targetHex)) return;

                Hex* currentCell = getCell();
                if (currentCell) {
                    currentCell->removeTroop();
                }

                setCell(targetHex);
                targetHex->setTroopOf<Ship>(this);
            }
    };
} //namespace GameLogic
