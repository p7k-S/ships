#pragma once
#include "GameLogic.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
// #include "ship.h"

namespace GameLogic {
    struct Treasure { std::string name; };
    struct Gold { uint16_t amount; };
    struct Empty {};
    struct SpecialItem {
        enum class Type { Compass, Map, Spyglass, Artifact };
        Type type;
        std::string description;
    };

    using HexContent = std::variant<Empty, Treasure, Gold, SpecialItem>;

    // hex coords
    class Hex {
        Ship* troop;         //only 1 allowed
        double noiseValue;
        CellType type;
    public:
        HexContent content;
        Owner owner;
        int q, r;

        Hex(int q, int r) : q(q), r(r), troop(nullptr), content(Empty{}) {}
        Hex(int q, int r, double noise) : q(q), r(r), noiseValue(noise), troop(nullptr), content(Empty{}) {}
        Hex(int q, int r, CellType t) : q(q), r(r), type(t), troop(nullptr), content(Empty{}) {}

        void setOwner(Owner owner) {
            if (type == CellType::LAND || type == CellType::FOREST) {
                owner = owner;
            } else {
                owner = GameLogic::Owner::FRIENDLY;
            }
        }
        Owner getOwner() { return owner; }

        void setShip(Ship* ship) { troop = ship; }
        void setTreasure(const std::string& name) { content = Treasure{name}; }
        void setGold(uint16_t amount) { content = Gold{amount}; }
        void setSpecialItem(SpecialItem::Type type, const std::string& desc) { content = SpecialItem{type, desc}; }
        void clearContent() { content = Empty{}; }

        uint16_t getGold() const {
            if (std::holds_alternative<Gold>(content)) {
                return std::get<Gold>(content).amount;
            }
            return 0;
        }
        uint16_t giveGold(uint16_t gold) {
            if (std::holds_alternative<Gold>(content)) {
                uint16_t toTake = std::min(std::get<Gold>(content).amount, gold);
                setGold(std::get<Gold>(content).amount - toTake);
                if (!getGold()) {
                    clearContent();
                }
                return toTake;
            }
            return 0;
        }

        void removeShip() { troop = nullptr; }
        bool hasShip() const { return troop != nullptr; }
        Ship* getShip() const { return troop; }

        sf::Vector2f getPosition(double hexRadius) const {
            double x = q * hexRadius * 1.5;
            double y = r * hexRadius * sqrt(3) + (q % 2) * hexRadius * sqrt(3) / 2.0;
            return sf::Vector2f(x, y);
        }

        bool hasTreasure() const { return std::holds_alternative<Treasure>(content); }
        bool hasGold() const { return std::holds_alternative<Gold>(content); }
        bool hasSpecialItem() const { return std::holds_alternative<SpecialItem>(content); }
        bool isEmpty() const { return std::holds_alternative<Empty>(content); }

        double getNoise() const { return noiseValue; }

        CellType getCellType() const { return type; }
        void setCellType(CellType t) { type = t; }
        bool isLand() const {
            return type == CellType::LAND || type == CellType::FOREST;
        }
    };
}
