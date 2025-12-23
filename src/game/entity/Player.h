#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include "BaseEntity.h"
#include "../items/BaseItem.h"
#include "../troops/BaseTroop.h"

namespace GameLogic {
    // возможно несколько игроков
    class Player : public Entity {
        private:
            uint8_t playerId;
            std::vector<uint8_t> friendsId;
            std::vector<Hex*> seenCells;
            std::vector<Hex*> viewableCells;
            /* // есть у каждой сущности
            sf::Color color; from Entity
            std::string name;
            std::vector<std::unique_ptr<Item>> items;
            std::vector<std::unique_ptr<Troop>> troops;
            std::vector<std::unique_ptr<Building>> buildings;
            */
            
            //stats
            // uint16_t kills;
            // float mapOpendPerc;
            // uint8_t placement;
            // uint8_t playersEliminated;
            // uint16_t goldFound;

            static int generateId() {
                static int counter = 1;
                return ++counter;
            }

            int x, y;

        public:
            Player(const std::string& name, sf::Color color) 
                : Entity(name, color), playerId(generateId()) {}

            //net
            void setPosition(int x, int y) { this->x = x; this->y = y; }
            // GETTERS
            std::string getName() const { return name; }
            uint8_t getId() const { return playerId; }
            std::vector<uint8_t> getFriendsIds() {
                return friendsId;
            }


            // SETTERS
            void appendFriendsId(uint8_t friendId) {
                friendsId.emplace_back(friendId);
            }
            void removeFriendsId(uint8_t friendId) {
                auto it = std::find(friendsId.begin(), friendsId.end(), friendId);
                if (it != friendsId.end()) {
                    friendsId.erase(it);
                }
            }
            
            // View
            void clearViewableCells() {
                viewableCells.clear();
            }
            void addViewableCells(Hex* cell) {
                viewableCells.push_back(cell);
            }
            std::vector<Hex*>& getViewableCells() {
                return viewableCells;
            }
            void addSeenCells(Hex* cell) {
                seenCells.push_back(cell);
            }
            std::vector<Hex*>& getSeenCells() {
                return seenCells;
            }
    };
}; // namespace GameLogic
