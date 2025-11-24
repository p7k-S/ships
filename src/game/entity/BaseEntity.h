#pragma once
#include "../GameConfig.h"
#include <memory>
#include <vector>
#include "../items/BaseItem.h"
#include "../troops/BaseTroop.h"

namespace GameLogic {
    class Entity {
        protected: 
            sf::Color color;
            std::string name;
            // std::vector<Troop*> troops;
            // std::vector<Item*> items;
            // std::vector<Building*> buildings;
            std::vector<std::unique_ptr<Item>> items;
            std::vector<std::unique_ptr<Troop>> troops;
            std::vector<std::unique_ptr<Building>> buildings;

        public:
            Entity(const std::string& NickName, sf::Color col) : name(NickName), color(col) {}

            sf::Color getColor() const { return color; }

            const std::vector<std::unique_ptr<Troop>>& getTroops() const {
                return troops;
            }
            const std::vector<std::unique_ptr<Building>>& getBuildings() const {
                return buildings;
            }
            const bool isDead() const {
                return troops.empty() && buildings.empty();
            }

            void setColor(sf::Color& newColor) { color = newColor; }

            void addTroop(std::unique_ptr<Troop> troop) {
                troops.push_back(std::move(troop));
            }

            // Удаление войска по указателю
            void removeTroop(Troop* troopToRemove) {
                auto it = std::find_if(troops.begin(), troops.end(),
                        [troopToRemove](const std::unique_ptr<Troop>& troop) {
                        return troop.get() == troopToRemove;
                        });

                if (it != troops.end()) {
                    troops.erase(it);
                }
            }

            void removeBuilding(Building* buildingToRemove) {
                auto it = std::find_if(buildings.begin(), buildings.end(),
                        [buildingToRemove](const std::unique_ptr<Building>& building) {
                        return building.get() == buildingToRemove;
                        });

                if (it != buildings.end()) {
                    buildings.erase(it);
                }
            }
            void addBuilding(std::unique_ptr<Building> building) {
                buildings.push_back(std::move(building));
            }

            // void removeTroop(Troop* troopToRemove) {
            //     auto it = std::find_if(troops.begin(), troops.end(),
            //             [troopToRemove](const std::unique_ptr<Troop>& troop) {
            //             return troop.get() == troopToRemove;
            //             });
            //
            //     if (it != troops.end()) {
            //         troops.erase(it);
            //     }
            // }

    };
}; // namespace GameLogic
