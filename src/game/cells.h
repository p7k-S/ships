#pragma once
#include "GameLogic.h"
#include "map.h"

namespace GameLogic {
    class HexCellType {
    public:
        virtual ~HexCellType() = default;
        virtual CellType getType() const = 0;
        virtual bool isPassable() const = 0;
        virtual int getMovementCost() const = 0;
    };

    class DeepWaterType : public HexCellType {
    public:
        CellType getType() const override { return CellType::DEEPWATER; }
        bool isPassable() const override { return true; }
        int getMovementCost() const override { return 2; }
    };

    class WATER : public HexCellType {
    public:
        CellType getType() const override { return CellType::WATER; }
        bool isPassable() const override { return true; }
        int getMovementCost() const override { return 1; }
    };

    class LAND : public HexCellType {
    public:
        CellType getType() const override { return CellType::LAND; }
        bool isPassable() const override { return true; }
        int getMovementCost() const override { return 1; }
    };

    class FOREST : public HexCellType {
    public:
        CellType getType() const override { return CellType::FOREST; }
        bool isPassable() const override { return true; }
        int getMovementCost() const override { return 2; }
    };
}
