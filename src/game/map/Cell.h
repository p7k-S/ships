#pragma once
#include "../GameLogic.h"
#include <any>
#include <cmath>
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
        std::vector<std::unique_ptr<Item>> resources;
        std::shared_ptr<Building> building;         //only 1 allowed
        std::shared_ptr<Troop> troop;         //only 1 allowed
        float noiseValue;
        CellType type;
    public:
        int q, r;

        // Hex(const Hex&) = delete;
        // Hex& operator=(const Hex&) = delete;
        // Hex(Hex&&) = default;
        // Hex& operator=(Hex&&) = default;


        Hex(int q, int r) : q(q), r(r), troop(nullptr) {}
        Hex(int q, int r, double noise) : q(q), r(r), noiseValue(noise), troop(nullptr) {}
        Hex(int q, int r, CellType t) : q(q), r(r), type(t), troop(nullptr) {}


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

         template<typename T, typename... Args>
            void addStackable(Args&&... args) {
                static_assert(std::is_base_of<Item, T>::value, "T must derive from Item");

                if constexpr (sizeof...(Args) == 1) {
                    if (T* existingItem = getItemOf<T>()) {
                        uint16_t amount = static_cast<uint16_t>(std::get<0>(std::make_tuple(args...)));
                        existingItem->addItemAmount(amount);
                        return;
                    }
                }

                items.push_back(std::make_unique<T>(std::forward<Args>(args)...));
            }

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



        uint16_t giveGold(uint16_t amount) {
            for (auto it = items.begin(); it != items.end(); ++it) {
                if (Gold* gold = dynamic_cast<Gold*>(it->get())) {
                    uint16_t currentAmount = gold->getItemAmount();
                    uint16_t toTake = std::min(currentAmount, amount);

                    if (toTake > 0) {
                        uint16_t newAmount = currentAmount - toTake;
                        gold->setItemAmount(newAmount);

                        if (newAmount == 0) {
                            items.erase(it);
                        }

                        return toTake;
                    }
                }
            }
            return 0;
        }

        std::unique_ptr<Item> giveItemByIndex(size_t index) {
            static_assert(!std::is_same_v<Item, Gold>, "Use giveGold() for Gold items");
            // Проверка валидности индекса
            if (index >= items.size()) {
                return nullptr;
            }

            // Освобождаем владение и возвращаем unique_ptr
            std::unique_ptr<Item> result = std::move(items[index]);
            items.erase(items.begin() + index);
            return result;
        }

        // troops
        template<typename T = Troop>
            void setTroopOf(std::shared_ptr<T> existingTroop) {
                static_assert(std::is_base_of_v<Troop, T>, "T must derive from Troop");
                if constexpr (std::is_same_v<T, Ship>) {
                    if (type >= CellType::LAND) {
                        return;
                    }
                }
                else if constexpr (std::is_same_v<T, Solder>) {
                    if (type <= CellType::WATER) {
                        return;
                    }
                }
                troop = std::move(existingTroop);
            }

        template<typename T>
            void setTroopOf(T& troopRef) {
                static_assert(std::is_base_of_v<Troop, T>, "T must derive from Troop");
                if constexpr (std::is_same_v<T, Ship>) {
                    if (type >= CellType::LAND) {
                        return;
                    }
                }
                else if constexpr (std::is_same_v<T, Solder>) {
                    if (type <= CellType::WATER) {
                        return;
                    }
                }
                troop = std::shared_ptr<Troop>(&troopRef); // Осторожно с управлением памятью!
            }

        bool hasTroop() const {
            return troop ? 1 : 0;
        }
        template<typename T>
            bool hasTroopOf() const {
                static_assert(std::is_base_of<Troop, T>::value, "T must derive from Troop");
                return troop && dynamic_cast<T*>(troop.get()) != nullptr;
            }

        template<typename T>
            T* getTroopOf() const {
                static_assert(std::is_base_of<Troop, T>::value, "T must derive from Troop");
                return troop ? dynamic_cast<T*>(troop.get()) : nullptr;
            }

        void removeTroop() {
            troop.reset();
        }


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
