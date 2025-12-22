#pragma once
#include <iostream>
#include "BaseTroop.h"

#include "../entity/Enemy.h"
#include "../entity/Player.h"
#include "../entity/Pirate.h"

namespace GameLogic {
    class Soldier : public Troop {
        private:
            uint8_t view = 3;         // range(радиус) = 5
            uint8_t move = 2;         // range(радиус) = 3
            uint16_t damage = 35;      // 35
            uint16_t damageRange = 1; // >= 1 (по дефолту 1 далее move - 2) !!не больше чем view
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
            Soldier(Owner owner, Hex* currCell):
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

            Type getType() const override { return Type::SOLDIER; }

            // bool areNeighbors(Hex* h1, Hex* h2);
            
            // Дамаг
            uint16_t getDamageRange() const override { return damageRange; }
            void setDamageRange(uint16_t range) override { damageRange = range; }
            virtual void takeDamage(uint16_t dmg) override { dmg > health ? health = 0 : health -= dmg; }
            virtual void giveDamageToTroop(Hex* targetCell) override {
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
            }
            virtual void giveDamageToBuilding(Hex* targetCell) override {
                Building* building = targetCell->getBuilding();
                building->takeDamage(damage);
                if (building->isDestroyed()) {
                    targetCell->removeBuilding();

                    std::cout << "building->isDestroyed()\n";
                    // lostResources(building);

                    std::visit(
                        [building](auto* ownerPtr) {
                            if (ownerPtr) {
                                ownerPtr->removeBuilding(building);
                            }
                        }, 
                        building->getOwner()
                    );

                    std::cout << "removed building\n";
                }
            }

            virtual bool isDestroyed() const override { return health == 0; }
            virtual void lostResources(Troop* enemy) override {
                Soldier* enemy_ship = static_cast<Soldier*>(enemy);

                if (enemy_ship->isDestroyed()) {
                    Hex* targetCell = enemy_ship->getCell();
                    uint16_t enemyGold = enemy_ship->getGold();
                    uint16_t availableSpace = getMaxGold() - getGold();
                    uint16_t transferableGold = std::min(enemyGold, availableSpace);

                    if (!hasItem()) {
                        addItem(enemy_ship->loseItem());
                    } else {
                        targetCell->addItem(enemy_ship->loseItem());
                    }


                    // Забираем сколько влезает
                    enemy_ship->loseGold(transferableGold);
                    takeGold(transferableGold);

                    // Остальное высыпаем на клетку
                    uint16_t remainingGold = enemy_ship->getGold();
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
            void setDamage(uint16_t dmg) override { damage = dmg; }
            void setHealth(uint16_t hp) override { health = hp; }

            void takeGold(uint16_t amount) override { amount > maxGold - gold ? gold = maxGold : gold += amount; }
            void loseGold(uint16_t amount) override { //amount <= gold ? gold -= amount : gold = 2; }
                if(amount <= gold) {
                    gold -= amount;
                }
            } // сколько надо отдать

            // Методы для предметов
            bool hasItem() const override { return item.get(); }
            Item* getItem() const { return item.get(); }

            void takeItemByIndexFromCell(size_t index) override {
                if (item) {
                    std::cout << "First remove item from ship\n";
                    return;
                }
                item = getCell()->giveItemByIndex(index);
            }

            std::unique_ptr<Item> loseItem() override {
                if (!item) return nullptr;
                return std::move(item);
            }

            virtual void addItem(std::unique_ptr<Item> newItem) override {
                if (newItem) {
                    item = (std::move(newItem));
                }
            }

            uint16_t getDamage() const override { return damage; }
            uint16_t getHealth() const override { return health; }
            uint16_t getMaxHealth() const override { return maxHealth; }
            uint16_t getGold() const override { return gold; }
            uint16_t getMaxGold() const override { return maxGold; }
            virtual uint8_t getView() const override { return view; }
            void setView(uint8_t range) override { view = range; }

            virtual uint8_t getMoveRange() const override { return move; }
            void setMoveRange(uint8_t range) override { move = range; }
            
            void addHP(uint16_t hp) override { hp > maxHealth - health ? health = maxHealth : health += hp; }

            // move
            virtual void moveTo(Hex* targetHex) override {
                if (!targetHex || !canMoveTo(targetHex)) return;

                Hex* currentCell = getCell();
                if (currentCell) {
                    currentCell->removeTroop();
                }

                setCell(targetHex);
                targetHex->setTroopOf<Soldier>(this);
            }
    };
} // namespace GameLogic
