#include "Game.h"
#include "../render/Colors.hpp"
#include "../render/sprites.h"
#include "../render/info_bars.h"

void Game::render() {
    window.clear();
    renderMap();
    renderShipRange();
    renderPath();
    renderShipUI();
    window.display();
}

void Game::renderMap() {
    for (const auto& hexp : seenCells) {
        // логика отрисовки гексов
    }
}
