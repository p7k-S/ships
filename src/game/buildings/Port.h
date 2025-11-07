#pragma once
#include "../GameLogic.h"
#include "../map/Cell.h"


// 15 клеток по горизонтали с разницей в ширину карты(15 = witdth) и ставим 2 порта
// 15 клеток вниз
// и тд

namespace GameLogic {
    class Port {
        private:
            Owner owner;
            uint8_t view;         // range(радиус) = 5
            // uint16_t damage;      // 35
            // uint16_t damageRange; // >= 1 (по дефолту 1 далее move - 2) !!не больше чем veiw
            uint16_t health;      // 100
            uint16_t maxHealth;
            uint16_t gold;
            uint16_t maxGold;     // 1000
            uint8_t treasure;        // по дефолту максимум перевозка 1 сокровища
            uint8_t spawnRate;      // кол-во ходов на spawn
            Hex* curCell;
        public:
            Port(Owner owner, Hex* currCell):
                owner(owner),
                curCell(currCell),
                view(5),
                health(1000),
                maxHealth(1000),
                gold(0),
                maxGold(5000),
                treasure(0),
                spawnRate(12)
        {}

        // Геттеры
        Owner getOwner() const { return owner; }
        uint8_t getView() const { return view; }
        uint16_t getHealth() const { return health; }
        uint16_t getMaxHealth() const { return maxHealth; }
        uint16_t getGold() const { return gold; }
        uint16_t getMaxGold() const { return maxGold; }
        uint8_t getTreasure() const { return treasure; }
        uint8_t getSpawnRate() const { return spawnRate; }
        Hex* getCurCell() const { return curCell; }
        const Hex* getCurCellConst() const { return curCell; }

        // Сеттеры с проверками
        void setOwner(Owner newOwner) { owner = newOwner; }

        void setView(uint8_t newView) { view = newView; }

        void setHealth(uint16_t newHealth) { health = (newHealth <= maxHealth) ? newHealth : maxHealth; }

        void setMaxHealth(uint16_t newMaxHealth) { 
            maxHealth = newMaxHealth;
            if (health > maxHealth) {
                health = maxHealth;
            }
        }

        void setGold(uint16_t newGold) { 
            gold = (newGold <= maxGold) ? newGold : maxGold;
        }

        void setMaxGold(uint16_t newMaxGold) { 
            maxGold = newMaxGold;
            if (gold > maxGold) {
                gold = maxGold;
            }
        }

        void setTreasure(uint8_t newTreasure) { 
            treasure = newTreasure; // Максимум 1 сокровище
        }

        void setSpawnRate(uint8_t newSpawnRate) { 
            if (newSpawnRate > 0) {
                spawnRate = newSpawnRate;
            }
        }

        void setCurCell(Hex* newCell) { 
            if (newCell != nullptr) {
                curCell = newCell;
            }
        }

        // Дополнительные методы для удобства
        void addGold(uint16_t amount) { 
            if (gold + amount <= maxGold) {
                gold += amount;
            } else {
                gold = maxGold;
            }
        }

        bool removeGold(uint16_t amount) { 
            if (amount <= gold) {
                gold -= amount;
                return true;
            }
            return false;
        }

        void takeDamage(uint16_t damage) { if (damage >= health) { health = 0; } else { health -= damage; } }

        void heal(uint16_t healAmount) { 
            if (health + healAmount <= maxHealth) {
                health += healAmount;
            } else {
                health = maxHealth;
            }
        }

        bool isAlive() const { return health > 0; }

        bool hasTreasure() const { return treasure > 0; }

        bool canStoreTreasure() const { return treasure == 0; }

        float getHealthPercentage() const { 
            return static_cast<float>(health) / static_cast<float>(maxHealth); 
        }

        // Методы для работы с сокровищами
        bool addTreasure() { 
            if (canStoreTreasure()) {
                treasure = 1;
                return true;
            }
            return false;
        }

        bool removeTreasure() { 
            if (hasTreasure()) {
                treasure = 0;
                return true;
            }
            return false;
        }

        // Метод для уменьшения счетчика спавна
        bool decrementSpawnRate() { 
            if (spawnRate > 0) {
                spawnRate--;
                return spawnRate == 0;
            }
            return false;
        }

        void resetSpawnRate(uint8_t baseRate = 12) { 
            spawnRate = baseRate;
        }
    };
} //namespace GameLogic
