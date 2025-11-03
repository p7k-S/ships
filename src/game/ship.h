#pragma once
#include "GameLogic.h"
#include "map.h"
#include <cstdint>

namespace GameLogic {
    class Ship {
        private:
            Owner owner;
            uint8_t view;         // range(радиус) = 5
            uint8_t move;         // range(радиус) = 3
            uint16_t damage;      // 35
            uint16_t health;      // 100
            uint16_t maxHealth;
            uint16_t gold;
            uint16_t maxGold;     // 1000
            bool treasure;        // по дефолту максимум перевозка 1 сокровища
            Hex* curCell;
        public:
            // Конструктор
            Ship(Owner owner, Hex* currCell):
                owner(owner),
                curCell(currCell),
                view(100),    // 4  2
                move(3),    // 2  1
                damage(50), // 30 20
                health(100),
                maxHealth(100),
                gold(100),
                maxGold(1000),
                treasure(false)
        {}

            // Map
            void setCell(Hex* cell) { curCell = cell; }
            Hex* getCell() const { return curCell; }

            static bool areNeighbors(Hex* h1, Hex* h2) {
                int dq = h2->q - h1->q;
                int dr = h2->r - h1->r;

                if (h1->q % 2 == 0) {
                    // Четный столбец
                    return (dq == 1 && dr == 0) || (dq == 1 && dr == -1) ||
                        (dq == 0 && dr == -1) || (dq == -1 && dr == -1) ||
                        (dq == -1 && dr == 0) || (dq == 0 && dr == 1);
                } else {
                    // Нечетный столбец
                    return (dq == 1 && dr == 1) || (dq == 1 && dr == 0) ||
                        (dq == 0 && dr == -1) || (dq == -1 && dr == 0) ||
                        (dq == -1 && dr == 1) || (dq == 0 && dr == 1);
                }
            }
            // Дамаг
            void takeDamage(uint16_t dmg) {
                if (dmg > health) {
                    health = 0;
                } else {
                    health -= dmg; 
                }
            }
            virtual void giveDamage(Hex* cell) {
                if (areNeighbors(curCell, cell) && cell->hasShip()) {
                    Ship* enemy = cell->getShip();
                    enemy->takeDamage(damage);
                    if (enemy->isDestroyed()) {
                        uint16_t enemyGold = enemy->getGold(), tmp = getMaxGold() - getGold();
                        uint16_t getPossibleGold = std::min(enemyGold, tmp);
                        enemy->loseGold(getPossibleGold);
                        takeGold(getPossibleGold);
                        addGoldToCell(cell, enemy->getGold());
                    }
                }
            }
            bool isEnemy() const { return owner == Owner::ENEMY || owner == Owner::PIRATE; }
            bool isDestroyed() const { return health == 0; }
            void Destroy() {
                if (curCell) {
                    Hex* temp = curCell;
                    curCell = nullptr;   // убираем ссылку на клетку
                    temp->removeShip();  // клетка больше не знает о корабле
                }
            }

            // Хилка
            void heal(uint16_t hp) { hp > maxHealth - health ? health = maxHealth : health += hp; }

            // Голда
            void addGoldToCell(Hex* cell, uint16_t amount) {
                if (!cell) return;
                if (cell->hasGold()) {
                    auto& g = std::get<Gold>(cell->content); // доступ к существующему Gold
                    g.amount += amount;
                } else {
                    if (amount) cell->setGold(amount);
                }
            }
            void takeGoldFromCell(Hex* cell) {
                if (!cell || !cell->hasGold()) return;
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

            // Методы для сокровища
            void takeTreasure() { treasure = true; }
            void giveTreasure() { treasure = false; }
            bool hasTreasure() const { return treasure; }

            Owner getOwner() const { return owner; }
            void setOwner(Owner owner) const { owner = owner; }
            uint16_t getDamage() const { return damage; }
            uint16_t getHealth() const { return health; }
            uint16_t getMaxHealth() const { return maxHealth; }
            uint16_t getGold() const { return gold; }
            uint16_t getMaxGold() const { return maxGold; }
            uint8_t getView() const { return view; }
            void setView(uint8_t range) { view = range; }

            // move
            bool canMoveTo(Hex* targetHex, std::vector<Hex>& hexMap) const {
                if (!targetHex || !curCell) return false;
                if (targetHex == curCell) return false;
                if (targetHex->isLand() || targetHex->hasShip()) return false;

                std::vector<Hex*> reachable = getCellsInRadius(*curCell, hexMap, curCell->getShip()->getMoveRange(), false);
                for (Hex* cell : reachable)
                    if (cell == targetHex)
                        return true;

                return false;
            }
            void moveTo(Hex* targetHex, std::vector<Hex>& hexMap) {
                if (!canMoveTo(targetHex, hexMap)) return;

                if (curCell) curCell->setShip(nullptr);
                curCell = targetHex;
                targetHex->setShip(this);
            }

            std::vector<Hex*> getCellsInRadius(Hex& start, std::vector<Hex>& hexMap, const uint8_t range, const bool isView) const;

            uint8_t getMoveRange() const { return move; }
            void setMoveRange(uint8_t range) { move = range; }
    };
    /*
    class PirateShip : public Ship {
        public:
            PirateShip(Hex* currCell, uint16_t gold = 800)
                : Ship(Owner::PIRATE, currCell, 5, 8, 45, 80, 80, gold, 1500, false)
            {}

            // Пираты наносят больше урона но имеют меньше здоровья
            void giveDamage(Hex* cell) override {
                if (areNeighbors(curCell, cell) && cell->hasShip()) {
                    Ship* enemy = cell->getShip();
                    // Пираты наносят на 50% больше урона противникам с золотом
                    uint16_t actualDamage = (enemy->getGold() > 0) ? damage * 1.5 : damage;
                    enemy->takeDamage(actualDamage);
                    if (enemy->isDestroyed()) {
                        // Пираты забирают все золото
                        uint16_t stolenGold = enemy->getGold();
                        enemy->giveGold(stolenGold);
                        takeGold(stolenGold);
                        addGoldToCell(cell, 0); // Очищаем клетку
                    }
                }
            }
    }; // пиратский корабль

    class EnemyShip : public Ship {
        public:
            EnemyShip(Hex* currCell)
                : Ship(Owner::ENEMY, currCell, 3, 7, 30, 120, 120, 200, 800, false)
            {}

            // Вражеские корабли более живучие
            void takeDamage(uint16_t dmg) override {
                // Вражеские корабли имеют 10% сопротивления урону
                uint16_t reducedDamage = dmg * 0.9;
                if (reducedDamage > health) {
                    health = 0;
                } else {
                    health -= reducedDamage; 
                }
            }

            void giveDamage(Hex* cell) override {
                if (areNeighbors(curCell, cell) && cell->hasShip()) {
                    Ship* enemy = cell->getShip();
                    enemy->takeDamage(damage);
                    if (enemy->isDestroyed()) {
                        // Вражеские корабли забирают только половину золота
                        uint16_t stolenGold = enemy->getGold() / 2;
                        enemy->giveGold(stolenGold);
                        takeGold(stolenGold);
                        addGoldToCell(cell, enemy->getGold()); // Оставляют половину на клетке
                    }
                }
            }
    }; // enemy ship
    */

} //namespace GameLogic
