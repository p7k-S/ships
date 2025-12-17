#pragma once
#include "BaseBuild.h"
#include "../items/BaseItem.h"
#include "../troops/BaseTroop.h"
#include "../map/Cell.h"
#include <memory>

namespace GameLogic {
    class Port : public Building {
        private:
            uint8_t view = 5;         // range(радиус) = 5
            // uint16_t damage;      // 35
            // uint16_t damageRange; // >= 1 (по дефолту 1 далее move - 2) !!не больше чем veiw
            uint16_t heal = 40;      // 100
            uint16_t health = 100;      // 100
            uint16_t maxHealth = 500;
            uint16_t gold = 500;
            uint16_t maxGold = 2000;     // 1000
            uint8_t spawnRate = 255;      // кол-во ходов на spawn
            std::vector<std::unique_ptr<Item>> items;
        public:
            Port(Owner owner, Hex* currCell):
                Building(owner, currCell) {}

        Type getType() const override { return Type::PORT; }

        // Геттеры
        uint8_t getView() const override { return view; }
        uint16_t getHealth() const override { return health; }
        uint16_t getHeal() const override { return heal; }
        uint16_t getMaxHealth() const override { return maxHealth; }
        uint16_t getGold() const { return gold; }
        uint16_t getMaxGold() const { return maxGold; }
        uint8_t getSpawnRate() const { return spawnRate; }

        // Сеттеры с проверками

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

        void setSpawnRate(uint8_t newSpawnRate) { 
            if (newSpawnRate > 0) {
                spawnRate = newSpawnRate;
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

        virtual void takeDamage(uint16_t damage) override { if (damage >= health) { health = 0; } else { health -= damage; } }
        virtual bool isDestroyed() const override { return health == 0; }

        virtual void lostResources(Troop* enemy) override;
        // virtual void lostResources(Troop* enemy) override {
        //     Hex* cell = getCell();
        //
        //     if (!enemy->hasItem() && !items.empty()) {
        //         enemy->addItem(std::move(items.front()));
        //         items.erase(items.begin());
        //     }
        //
        //     while (!items.empty()) {
        //         if (cell) {
        //             cell->addItem(std::move(items.front()));
        //         }
        //         items.erase(items.begin());
        //     }
        // }


        // virtual void lostResources(Port* enemy) override {
        //     Port* enemy_port = static_cast<Port*>(enemy);
        //
        //     if (enemy_port->isDestroyed()) {
        //         uint16_t enemyGold = enemy_port->getGold();
        //         uint16_t availableSpace = getMaxGold() - getGold();
        //         uint16_t transferableGold = std::min(enemyGold, availableSpace);
        //
        //         // Забираем сколько влезает
        //         enemy_port->loseGold(transferableGold);
        //         takeGold(transferableGold);
        //
        //         // Остальное высыпаем на клетку
        //         uint16_t remainingGold = enemy_port->getGold();
        //         Hex* targetCell = enemy_port->getCell();
        //         if (remainingGold > 0) {
        //             addGoldToCell(targetCell, remainingGold);
        //             enemy_port->loseGold(remainingGold);
        //         }
        //     }
        // }


        void healing() { 
            if (health + heal <= maxHealth) {
                health += heal;
            } else {
                health = maxHealth;
            }
        }

        void healingTroops(uint16_t healAmount) { 
        }

        bool isAlive() const { return health > 0; }

        float getHealthPercentage() const { 
            return static_cast<float>(health) / static_cast<float>(maxHealth); 
        }

        // Методы для работы с сокровищами
        // bool addTreasure() { 
        //     // playerу добавить
        // }

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

        bool isPort() const override { return true; }

        void addItem(std::unique_ptr<Item> newItem) {
            items.push_back(std::move(newItem));
        }
        uint16_t getItemsSize() const {
            return items.size();
        }

    };
} //namespace GameLogic
