#pragma once
#include "../GameLogic.h"
#include "../map/Cell.h"

namespace GameLogic {
    class Ship {
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
            bool treasure;        // по дефолту максимум перевозка 1 сокровища
            Hex* curCell;
        public:
            // Конструктор
            Ship(Owner owner, Hex* currCell):
                owner(owner),
                curCell(currCell),
                view(255),    // 4  2
                move(4),    // 2  1
                damage(50), // 30 20
                damageRange(3),
                health(100),
                maxHealth(100),
                gold(200),
                maxGold(1000),
                treasure(false)
        {}

            // Map
            void setCell(Hex* cell) { curCell = cell; }
            Hex* getCell() const { return curCell; }

            static std::vector<Hex*> getShortestRoad(std::vector<Hex>& area, Hex* start, Hex* end);
            static std::vector<Hex*> getShortestRoad(std::vector<Hex*>& area, Hex* start, Hex* end);
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
            virtual void giveDamage(Hex* cell, std::vector<Hex>& hexMap) {
                if (cell == curCell) return;
                std::vector<Hex*> reachable = cellsInRange(*curCell, hexMap, curCell->getShip()->getDamageRange(), RangeMode::DAMAGE); // false=не радиус обзора

                if (std::find(reachable.begin(), reachable.end(), cell) != reachable.end()) {
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
                    uint16_t currentGold = cell->getGold();
                    cell->setGold(currentGold + amount);
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
            void setOwner(Owner newOwner) { owner = newOwner; }
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

                std::vector<Hex*> reachable = cellsInRange(*curCell, hexMap, curCell->getShip()->getMoveRange(), RangeMode::MOVE);
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

            std::vector<Hex*> cellsInRange(Hex& start, std::vector<Hex>& hexMap, uint8_t maxMoves, const RangeMode mode) const; // ship.cpp

            uint8_t getMoveRange() const { return move; }
            void setMoveRange(uint8_t range) { move = range; }

            Hex* getNeighborHex(Hex* hex, int side, const std::vector<Hex*>& allHexes);
            struct HexEdge;
            struct HexEdgeHash;
            std::vector<HexEdge> getHexEdges(Hex* hex);
            std::vector<HexEdge> getPerimeterEdges(const std::vector<Hex*>& area, const std::vector<Hex*>& allHexes);
            std::vector<Hex*> getBorderHexes(const std::vector<Hex*>& area, const std::vector<Hex*>& allHexes);
            std::vector<Hex*> getBorderHexesWithNeighbors(const std::vector<Hex*>& area, const std::vector<Hex>& allHexes);
    };
} //namespace GameLogic
