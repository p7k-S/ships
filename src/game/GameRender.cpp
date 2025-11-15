#include "Game.h"
#include "../render/Colors.hpp"
#include "../render/sprites.h"
#include "../render/info_bars.h"

#include "map/GenerateMap.h"
#include "map/Cell.h"

#include "GameLogic.h"
#include "items/Treasure.h" // Добавьте этот include

#include "GameConfig.h"
#include "GameLogic.h"
#include <vector>
// #include <iostream>
#include "../render/ui/UIRenderer.h"

void Game::render() {
    window.clear(sf::Color(20, 20, 20));

    // 1️⃣ Устанавливаем карту, если режим полноэкранный
    if (fullscreenMapMode) {
        window.setView(mapView);
    } else {
        UIRenderer::renderSidebar(window, font, "lolo");
        UIRenderer::renderBottomBar(window, font, totalTurnCount);
        // renderCellInfoPanel();
        // renderBottomStatsBar();
    }

    // 2️⃣ Основные компоненты рендеринг
    renderMap(); // отрисовка видимых клеток
    renderShipRange();
    renderPath();
    renderShipUI(); // отрисовка бара у кораблей

    // 3️⃣ Вернуть стандартный view, чтобы UI рисовался в фиксированных координатах экрана
    // window.setView(window.getDefaultView());

    // Отрисовка UI

    window.display();

}


void Game::renderBottomStatsBar() {
    sf::RectangleShape bar;
    bar.setSize({(float)window.getSize().x, 200.f});
    bar.setFillColor(sf::Color(50, 50, 70));
    window.draw(bar);

    sf::Text stats("Ships: 5 | Treasures: 2 | Turn: 3", font, 20);
    stats.setPosition(20, 20);
    window.draw(stats);
}

void Game::renderCellInfoPanel() {
    sf::RectangleShape panel;
    panel.setSize({(float)window.getSize().x * 0.25f, (float)window.getSize().y * 0.8f});
    panel.setFillColor(sf::Color(40, 40, 60));
    window.draw(panel);

    if (targetHex) {
        sf::Text info("Cell: " + std::to_string(targetHex->q) + "," +
                std::to_string(targetHex->r), font, 18);
        info.setPosition(20, 20);
        window.draw(info);
    }
}

void Game::renderMap() {
    // Получаем видимую область камеры
    sf::FloatRect viewBounds = getViewBounds();
    
    for (const auto& hexp : players[p_id]->getSeenCells()) {
        const auto& hex = *hexp;
        double x_pos = hex.q * hexRadius * 1.5;
        double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
        
        // Проверяем, находится ли гекс в области видимости
        if (!isHexInView(x_pos, y_pos, hexRadius, viewBounds)) {
            continue; // Пропускаем невидимые гексы
        }
        
        renderHex(hex, x_pos, y_pos);
    }
}

bool Game::isHexInView(float x, float y, float radius, const sf::FloatRect& viewBounds) {
    sf::FloatRect hexBounds(x, y, radius * 2, radius * 2);
    return viewBounds.intersects(hexBounds);
}

sf::FloatRect Game::getViewBounds() {
    sf::View currentView = window.getView();
    sf::Vector2f center = currentView.getCenter();
    sf::Vector2f size = currentView.getSize();
    return sf::FloatRect(center.x - size.x/2, center.y - size.y/2, size.x, size.y);
}

void Game::renderPath() {
    for (auto* hex : currentPath) {
        sf::CircleShape marker(hexRadius / 3);
        marker.setFillColor(sf::Color(100, 200, 255, 120));
        marker.setOrigin(marker.getRadius(), marker.getRadius());
        
        float hx = hex->q * hexRadius * 1.5f + 50;
        float hy = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0f + 50;
        marker.setPosition(hx, hy);
        
        window.draw(marker);
    }
}

void Game::renderHex(const gl::Hex& hex, float x_pos, float y_pos) {
    sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);

    // Определяем видимость и цветовую схему
    std::vector<gl::Hex*> view_cells = players[p_id]->getViewableCells();
    bool isVisible = std::find(view_cells.begin(), view_cells.end(), &hex) != view_cells.end();
    auto currentScheme = isVisible ? colSchemeDefault : colSchemeInactive;

    hexShape.setFillColor(getColorByScheme(hex.getNoise(), currentScheme, deepWater, water, land));
    hexShape.setOutlineColor(COLORS[isVisible ? "dark_gray" : "very_dark_gray"]);
    hexShape.setOutlineThickness(1);

    // Подготовка спрайтов
    sf::Sprite goldSprite, shipSprite;
    bool hasGold = false;
    bool hasTreasure = false;
    bool hasTroop = false;
    
    if (isVisible) {
        hasTroop = hex.hasTroop();
        hasTreasure = hex.hasItemOf<gl::Treasure>();
        hasGold = hex.hasGold();
        
        if (hasTroop) {
            renderShipOnHex(hex, hexShape, shipSprite);
        } else if (hasTreasure) {
            goldSprite.setTexture(treasure_texture);
        } else if (hasGold) {
            goldSprite.setTexture(gold_texture);
        }
        
        // Нормализуем только используемые спрайты
        if (hasTreasure || hasGold) {
            normlaizeSprite(goldSprite, hexRadius, x_pos, y_pos);
        }
        if (hasTroop) {
            normlaizeSprite(shipSprite, hexRadius, x_pos, y_pos);
        }
    }

    // Выделение выбранного гекса
    if (!selectedTroop && selectedHex && hex.q == selectedHex->q && hex.r == selectedHex->r) {
        hexShape.setOutlineColor(COLORS["deep_yellow"]);
        hexShape.setOutlineThickness(2);
    }

    // Отрисовка всего за один проход
    window.draw(hexShape);
    
    if (hasTreasure || hasGold) {
        window.draw(goldSprite);
    }
    
    if (hasGold) {
        drawResourceText(window, hex, x_pos + 50, y_pos + 50, hexRadius, font, font_size);
    }
    
    if (hasTroop) {
        window.draw(shipSprite);
    }
}

void Game::renderShipRange() {
    if (!selectedTroop) return;
    
    gl::Hex* selectedHex = selectedTroop->getCell();
    if (!selectedHex) return;

    // Диапазон перемещения
    std::vector<gl::Hex*> reachableHexes = cellsInRange(
        *selectedHex, hexMap, selectedTroop->getMoveRange(), gl::RangeMode::MOVE);
    for (gl::Hex* reachableHex : reachableHexes) {
        renderRangeHex(reachableHex, sf::Color(100, 255, 100, 80), sf::Color(112, 129, 88, 255));
    }

    // Диапазон атаки
    std::vector<gl::Hex*> attackRangeHexes = cellsInRange(
        *selectedHex, hexMap, selectedTroop->getDamageRange(), gl::RangeMode::DAMAGE);
    for (gl::Hex* attackHex : attackRangeHexes) {
        renderRangeHex(attackHex, sf::Color(200, 40, 40, 50), sf::Color::Transparent);
    }
}

void Game::renderRangeHex(gl::Hex* hex, sf::Color fillColor, sf::Color outlineColor) {
    double x_pos = hex->q * hexRadius * 1.5;
    double y_pos = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0;

    sf::ConvexShape rangeShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
    rangeShape.setFillColor(fillColor);
    rangeShape.setOutlineColor(outlineColor);
    rangeShape.setOutlineThickness(1);
    window.draw(rangeShape);
}

void Game::renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite) {
    auto* ship = hex.getTroop();
    if (!ship) return;
    auto shipOwner = ship->getOwner();

    if (isPlayerOwner(shipOwner)) {
        shipSprite.setTexture(player_ship_texture);
        hexShape.setOutlineColor(sf::Color::Green);
    } else if (isEnemyOwner(shipOwner)) {
        shipSprite.setTexture(enemy_ship_texture);
        hexShape.setOutlineColor(COLORS["burgundy"]);
    } else if (isPirateOwner(shipOwner)) {
        shipSprite.setTexture(pirate_ship_texture);
        hexShape.setOutlineColor(sf::Color::Black);
    }

        // case gl::Owner::FRIENDLY:
        //     hexShape.setOutlineColor(sf::Color::White);
        //     break;
    // }
    
    if (colSchemeDefault == INVERT) {
        if (isPirateOwner(shipOwner)) {
                    hexShape.setFillColor(COLORS["very_dark_gray"]);
        } else if (isEnemyOwner(shipOwner)) {
                    hexShape.setFillColor(COLORS["burgundy"]);
        } else if (isPlayerOwner(shipOwner)) {
                    hexShape.setFillColor(players[p_id]->getColor());
                // case gl::Owner::FRIENDLY:
                //     hexShape.setFillColor(sf::Color::White);
                //     break;
        }
    }
}

void Game::renderShipUI() {
    std::vector<gl::Hex*> view_cells = players[p_id]->getViewableCells();
    for (const auto& hexp : players[p_id]->getSeenCells()) {
        const auto& hex = *hexp;
        bool isVisible = std::find(view_cells.begin(), view_cells.end(), hexp) != view_cells.end();
        
        if (hex.hasTroop() && isVisible) {
            double x_pos = hex.q * hexRadius * 1.5;
            double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
            
            if (colSchemeDefault == COLORFULL) {
                drawShipBar(window, static_cast<gl::Ship*>(hex.getTroop()), x_pos + 50, y_pos + 50, hexRadius, font, font_size);
            }
        }
    }
}
