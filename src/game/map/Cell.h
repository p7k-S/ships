#pragma once
#include "../GameLogic.h"
#include <any>
#include <cmath>
#include <cstdint>
#include <memory>
#include <variant>
#include "../items/BaseItem.h"
#include "../items/Gold.h"

#include "../troops/BaseTroop.h"
#include "../buildings/BaseBuild.h"
#include "../GameConfig.h"

namespace GameLogic {
    class Hex {
        std::vector<std::unique_ptr<Item>> items;
        Building* building = nullptr;
        Troop* troop = nullptr;
        uint16_t gold = 0;
        float noiseValue;
        CellType type;
    public:
        int q, r;

        Hex(int q, int r) : q(q), r(r) {}
        Hex(int q, int r, double noise) : q(q), r(r), noiseValue(noise) {}
        Hex(int q, int r, CellType t) : q(q), r(r), type(t) {}


        // GOLD
        void addGold(uint16_t amount) {
            gold += amount;
        }
        bool hasGold() const {
            return gold ? true : false;
        }
        bool getGold() const {
            return gold;
        }
        uint16_t giveGold(uint16_t amount) {
            uint16_t toTake = std::min(gold, amount);
            gold -= toTake;
            return toTake;
        }



        // items
        template<typename T>
            T* getItemOf() const {
                static_assert(std::is_base_of<Item, T>::value, "T must derive from Item");

                for (auto& item : items) {
                    if (auto derived = dynamic_cast<T*>(item.get())) {
                        return derived;
                    }
                }
                return nullptr;
            }

        template<typename T>
            bool hasItemOf() const {
                auto* item = getItemOf<T>();
                return item != nullptr;
            }

         // template<typename T, typename... Args>
         //    void addStackable(Args&&... args) {
         //        static_assert(std::is_base_of<Item, T>::value, "T must derive from Item");
         //
         //        if constexpr (sizeof...(Args) == 1) {
         //            if (T* existingItem = getItemOf<T>()) {
         //                uint16_t amount = static_cast<uint16_t>(std::get<0>(std::make_tuple(args...)));
         //                existingItem->addItemAmount(amount);
         //                return;
         //            }
         //        }
         //
         //        items.push_back(std::make_unique<T>(std::forward<Args>(args)...));
         //    }

        // Items
         template<typename T, typename... Args>
             void addItem(Args&&... args) {
                 static_assert(std::is_base_of_v<Item, T>, "T must derive from Item");
                 items.push_back(std::make_unique<T>(std::forward<Args>(args)...));
             }

             // Для передачи готового unique_ptr
             void addItem(std::unique_ptr<Item> existingItem) {
                 if (existingItem) {
                     items.push_back(std::move(existingItem));
                 }
             }

        std::unique_ptr<Item> giveItemByIndex(size_t index) {
            static_assert(!std::is_same_v<Item, Gold>, "Use giveGold() for Gold items");
            if (index >= items.size()) {
                return nullptr;
            }

            std::unique_ptr<Item> result = std::move(items[index]);
            items.erase(items.begin() + index);
            return result;
        }

        // troops
        template<typename T>
            bool setTroopOf(T* troopPtr) {
                static_assert(std::is_base_of_v<Troop, T>, "T must derive from Troop");

                if (troop) { return false; }

                if constexpr (std::is_same_v<T, Ship>) {
                    if (type >= CellType::LAND) {
                        return false;
                    }
                }
                else if constexpr (std::is_same_v<T, Solder>) {
                    if (type <= CellType::WATER) {
                        return false;
                    }
                }

                troop = troopPtr;
                return true;
            }

        bool hasTroop() const { return troop ? true : false; }
        Troop* getTroop() const { return hasTroop() ? troop : nullptr; }
        void removeTroop() { troop = nullptr; }

        // template<typename T>
        //     bool hasTroopOf() const {
        //         static_assert(std::is_base_of_v<Troop, T>, "T must derive from Troop");
        //         return troop && dynamic_cast<const T*>(troop) != nullptr;
        //         // было: troop.get() | стало: troop
        //     }

        // ✅ Исправленный getTroopOf  
        // template<typename T>
        //     T* getTroopOf() const {
        //         static_assert(std::is_base_of_v<Troop, T>, "T must derive from Troop");
        //         return troop ? dynamic_cast<T*>(troop) : nullptr;
        //         // было: troop.get() | стало: troop
        //     }

        // ✅ Исправленный removeTroop



        // Noise
        float getNoise() const { return noiseValue; }

        // CellType
        CellType getCellType() const { return type; }
        void setCellType(CellType t) { type = t; }
        bool isLand() const {
            return type == CellType::LAND || type == CellType::FOREST;
        }

        // перегрууууууузкаааааааааа 5G
        bool operator==(const Hex& other) const {
            return q == other.q && r == other.r;
        }
    };
} // namespace GameLogic
