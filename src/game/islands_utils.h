#pragma once
#include "map.h"
#include <vector>
#include "GameLogic.h"

namespace GameLogic {
    inline bool isIslandBorder(int q, int r,
            const std::vector<std::vector<Hex>>& map,
            int W, int H) 
    {
        if (!map[q][r].isLand()) return false;

        for (auto [dq, dr] : DIRECTIONS) {
            int nq = q + dq;
            int nr = r + dr;

            if (nq < 0 || nq >= W || nr < 0 || nr >= H) continue;

            if (!map[nq][nr].isLand()) return true;
        }
        return false;
    }

}
