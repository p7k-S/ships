#pragma once
#include "../GameLogic.h"
#include <cmath>
#include <variant>

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
    // using HexTroop = std::variant<Empty, Treasure, Gold, SpecialItem>;

    // hex coords
    class Hex {
        HexContent content;
        Ship* troop;         //only 1 allowed
        double noiseValue;
        CellType type;
    public:
        int q, r;

        Hex(int q, int r) : q(q), r(r), troop(nullptr), content(Empty{}) {}
        Hex(int q, int r, double noise) : q(q), r(r), noiseValue(noise), troop(nullptr), content(Empty{}) {}
        Hex(int q, int r, CellType t) : q(q), r(r), type(t), troop(nullptr), content(Empty{}) {}

        // Content
        bool hasTreasure() const { return std::holds_alternative<Treasure>(content); }
        void setTreasure(const std::string& name) { content = Treasure{name}; }
        bool hasSpecialItem() const { return std::holds_alternative<SpecialItem>(content); }
        void setSpecialItem(SpecialItem::Type type, const std::string& desc) { content = SpecialItem{type, desc}; }
        bool isEmpty() const { return std::holds_alternative<Empty>(content); }
        void setEmpty() { content = Empty{}; }

        // Owner
        // void setOwner(Owner newOwner) {
        //     if (type == CellType::LAND || type == CellType::FOREST) {
        //         owner = newOwner;
        //     } else {
        //         owner = GameLogic::Owner::FRIENDLY;
        //     }
        // }
        //
        // Owner getOwner() { return owner; }

        // ship
        void setShip(Ship* ship) {
            if (type != CellType::DEEPWATER && type != CellType::WATER) {
                return;
            }
            troop = ship;
        }
        void removeShip() { troop = nullptr; }
        bool hasShip() const { return troop != nullptr; }
        Ship* getShip() const { return troop; }

        // Noise
        double getNoise() const { return noiseValue; }

        // CellType
        CellType getCellType() const { return type; }
        void setCellType(CellType t) { type = t; }
        bool isLand() const {
            return type == CellType::LAND || type == CellType::FOREST;
        }

        // Gold
        bool hasGold() const { return std::holds_alternative<Gold>(content); }

        void setGold(uint16_t amount) { content = Gold{amount}; }
        uint16_t getGold() const {
            return hasGold() ? std::get<Gold>(content).amount : 0;
        }

        uint16_t giveGold(uint16_t gold) {
            if (hasGold()) {
                uint16_t toTake = std::min(getGold(), gold);
                setGold(getGold() - toTake);
                if (!getGold()) {
                    setEmpty();
                }
                return toTake;
            }
            return 0;
        }

        bool operator==(const Hex& other) const {
            return q == other.q && r == other.r;
        }

    };
} // namespace GameLogic
