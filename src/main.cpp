#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
// #include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>
#include <iostream>
#include "game/GameLogic.h"

#include "game/map/Cell.h"
#include "game/map/PerlinNoise.h"
#include "game/map/MapReneder.h"
#include "game/troops/Ship.h"

#include "game/constants.h"
#include "render/Colors.hpp"
#include "render/info_bars.h"
#include "render/sprites.h"

#include "render/ui/startUI.h"
#include "game/constants.h"


namespace gl = GameLogic;

// https://chat.deepseek.com/a/chat/s/abf7b889-2588-41ac-ace5-f8f42eda92f9
int main() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    SimpleConfigUI configUI;
    if (configUI.show()) {
    }

    PerlinNoise perlin(seed);
    std::vector<gl::Hex> hexMap = createMap(perlin, mapWidth, mapHeight, octaves, scale);

    sf::Font font;
    if (!font.loadFromFile(font_path)) { std::cerr << "Не удалось загрузить шрифт из: " << font_path << std::endl; }

    double deepWater;
    double water;
    double land;
    {
    std::vector<double> noiseValues = getNoises(hexMap);
    std::vector<double> sortedValues = noiseValues;
    std::sort(sortedValues.begin(), sortedValues.end());
    deepWater = sortedValues[noiseValues.size() * deepWater_delim];
    water = sortedValues[noiseValues.size() * water_delim];
    land = sortedValues[noiseValues.size() * land_delim];
    }

    std::vector<gl::Hex*> deepWaterHexes;
    std::vector<gl::Hex*> waterHexes;
    std::vector<gl::Hex*> landHexes;
    std::vector<gl::Hex*> forestHexes;

    int i = 0;
    for (auto& hex : hexMap) {
        // std::cout << "(" << hex.q << ", " << hex.r << ")" << std::endl;
        double value = hex.getNoise();
        gl::CellType type;
        if (value <= deepWater) {
            type = gl::CellType::DEEPWATER;
            deepWaterHexes.push_back(&hex);
        } else if (value <= water) {
            type = gl::CellType::WATER;
            waterHexes.push_back(&hex);
        } else if (value <= land) {
            type = gl::CellType::LAND;
            landHexes.push_back(&hex);
        } else {
            type = gl::CellType::FOREST;
            forestHexes.push_back(&hex);
        }
        hex.setCellType(type);
    }

    // std::vector<std::vector<gl::Hex*>> islands = gl::findIslands(hexMap, mapWidth, mapHeight);
    // std::vector player_island = islands.front();
    // std::vector enemy_island = islands.back();
    // std::uniform_int_distribution<> dist(0, player_island.size() - 1);
    // gl::Hex* cur_pos = player_island[dist(gen)];
    // gl::Ship newShip(gl::Owner::PLAYER, cur_pos);

    std::srand(std::time(nullptr));
    std::vector<std::unique_ptr<gl::Ship>> ships;

    for (int i = 0; i < (waterHexes.size() + deepWaterHexes.size()) * 0.02; ++i) {
        gl::Owner ow = gl::Owner::PIRATE;
        if (!i) {
            ow = gl::Owner::PLAYER;
        } else if (i%2) {
            ow = gl::Owner::ENEMY;
        }
        int randomIndex = std::rand() % waterHexes.size();
        gl::Hex* startHex = waterHexes[randomIndex];
        bool exists = false;
        for (const auto& ship : ships) {
            if (ship->getCell() == startHex) { exists = true; break; }
        }
        if (exists) { --i; continue; } 


        // Создаем корабль и добавляем в вектор
        auto myShip = std::make_unique<gl::Ship>(ow, startHex);
        startHex->setShip(myShip.get());

        // Перемещаем владение в вектор
        ships.push_back(std::move(myShip));

    }
    std::vector<gl::Hex*> seenCells;
    addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);

    std::uniform_int_distribution<> goldDist(10, 100);
    std::uniform_real_distribution<> chanceDist(0.0, 1.0);
    for (auto& hex : hexMap) {
        if (chanceDist(gen) < 0.15) {
            hex.setGold(goldDist(gen));
        }
    }

    int treasuresToPlace = 3;
    std::uniform_int_distribution<> indexDist(0, hexMap.size() - 1);
    std::unordered_set<int> usedIndices;

    while (treasuresToPlace > 0 && usedIndices.size() < hexMap.size()) {
        int randomIndex = indexDist(gen);
        if (usedIndices.insert(randomIndex).second) {
            hexMap[randomIndex].setTreasure("nice");
            treasuresToPlace--;
        }
    }



    sf::Texture player_ship_texture;
    if (!player_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/player_ship.png")) {
    }
    sf::Texture pirate_ship_texture;
    if (!pirate_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/pirates_ship.png")) {
    }
    sf::Texture enemy_ship_texture;
    if (!enemy_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/enemy_ship.png")) {
    }
    sf::Texture gold_texture;
    if (!gold_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/gold_cons.png")) {
    }
    sf::Texture treasure_texture;
    if (!treasure_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/treasure.png")) {
    }


    // cur_pos.troop = true; 


    /*
    std::vector<std::vector<gl::Hex>> sorted_islands = islands;
    std::sort(sorted_islands.begin(), sorted_islands.end(), 
            [](const auto& a, const auto& b) { return a.size() < b.size(); });
    */


    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default, settings);

    sf::View view = window.getDefaultView();
    sf::View defaultView = view;

    gl::Hex* selectedHex = nullptr;
    gl::Ship* selectedShip = nullptr;
    bool waitingForMove = false;
    bool isDragging = false;
    sf::Vector2f lastMousePos;

    std::vector<gl::Hex*> currentPath;
    gl::Hex* targetHex = nullptr; // Новая переменная для хранения целевой клетки

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
                colScheme = (colScheme == COLORS) ? INVERT : COLORS;
                colSchemeInactive = (colSchemeInactive == DARK_COLORS) ? INVERT : DARK_COLORS;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (waitingForMove && selectedShip && targetHex) {
                    bool actionPerformed = false;

                    // --- Перемещение ---
                    if (selectedShip->canMoveTo(targetHex, hexMap)) {
                        selectedShip->moveTo(targetHex, hexMap);
                        selectedShip->takeGoldFromCell(targetHex);
                        addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);
                        std::cout << "Корабль перемещен на (" << targetHex->q << ", " << targetHex->r << ")" << std::endl;
                        actionPerformed = true;
                    }
                    // --- Атака вражеского корабля ---
                    else if (targetHex->getShip() && targetHex->getShip()->getOwner() != gl::Owner::FRIENDLY) {
                        selectedShip->giveDamage(targetHex, hexMap);
                        std::cout << "Атакован вражеский корабль на (" << targetHex->q << ", " << targetHex->r << ")" 
                            << " hp: " << targetHex->getShip()->getHealth() 
                            << " my hp " << selectedShip->getHealth() << std::endl;

                        if (targetHex->getShip()->isDestroyed()) {
                            std::cout << "Корабль противника уничтожен!" << std::endl;
                        }
                        actionPerformed = true;
                    }

                    if (actionPerformed) {
                        waitingForMove = false;
                        selectedShip = nullptr;
                        selectedHex = nullptr;
                        targetHex = nullptr;
                        currentPath.clear();
                    } else {
                        std::cout << "Невозможно выполнить действие" << std::endl;
                    }
                }
            }

            // Обработка зума и перемещения камеры (остается без изменений)
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    view.zoom(0.9f);
                } else {
                    view.zoom(1.1f);
                }
                window.setView(view);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left)
                    view.move(-10.f, 0.f);
                if (event.key.code == sf::Keyboard::Right)
                    view.move(10.f, 0.f);
                if (event.key.code == sf::Keyboard::Up)
                    view.move(0.f, -10.f);
                if (event.key.code == sf::Keyboard::Down)
                    view.move(0.f, 10.f);
                if (event.key.code == sf::Keyboard::Equal)
                    view.zoom(0.9f);
                if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract)
                    view.zoom(1.1f);
                if (event.key.code == sf::Keyboard::R)
                    view = defaultView;

                window.setView(view);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                isDragging = true;
                lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isDragging = false;
            }

            if (event.type == sf::Event::MouseMoved && isDragging) {
                sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                sf::Vector2f delta = lastMousePos - currentMousePos;
                view.move(delta);
                window.setView(view);
                lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            }

            // Обработка клика мыши для выбора цели
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                for (auto& hex : hexMap) {
                    float hx = hex.q * hexRadius * 1.5 + 50;
                    float hy = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0 + 50;

                    float dx = worldPos.x - hx;
                    float dy = worldPos.y - hy;

                    if (std::sqrt(dx*dx + dy*dy) <= hexRadius) {
                        if (waitingForMove && selectedShip) {
                            // Выбор цели для перемещения/атаки
                            targetHex = &hex;
                            std::vector<gl::Hex*> reachableHexes = selectedShip->cellsInRange(*selectedShip->getCell(), hexMap, selectedShip->getMoveRange(), gl::RangeMode::MOVE);
                            // currentPath = Ship::getShortestRoad(reachableHexes, selectedShip->getCell(), targetHex);

                            if (targetHex->getShip() && targetHex->getShip()->getOwner() != gl::Owner::FRIENDLY) {
                                std::cout << "Цель выбрана: атака вражеского корабля на (" 
                                    << targetHex->q << ", " << targetHex->r << ")" << std::endl;
                            } else {
                                std::cout << "Цель выбрана: перемещение на (" 
                                    << targetHex->q << ", " << targetHex->r << ")" << std::endl;
                            }
                            std::cout << "Нажмите Enter для подтверждения действия или выберите другую цель" << std::endl;

                        } else {
                            // Выбор корабля
                            if (hex.getShip() && hex.getShip()->getOwner() == gl::Owner::PLAYER) {
                                selectedShip = hex.getShip();
                                waitingForMove = true;
                                targetHex = nullptr;
                                currentPath.clear();
                                std::cout << "Корабль выбран. Выберите цель для перемещения/атаки" << std::endl;
                            } else {
                                selectedHex = &hex;
                                selectedShip = nullptr;
                                waitingForMove = false;
                                targetHex = nullptr;
                                currentPath.clear();
                            }
                        }
                        break;
                    }
                }
            }
        }


        // удаляем мертвых
        for (auto& ship : ships) {
            if (ship->isDestroyed()) {
                ship->Destroy();
            }
        }
        ships.erase(
                std::remove_if(ships.begin(), ships.end(),
                    [](const std::unique_ptr<gl::Ship>& s) { return s->isDestroyed(); }),
                ships.end());

        // клетки которые можем увидеть видим состояние их
        std::vector<gl::Hex*> vieweableHexes;
        std::unordered_set<gl::Hex*> uniqueCells;
        for (auto& ship : ships) {
            if (ship->getOwner() == gl::Owner::PLAYER) {
                std::vector<gl::Hex*> visibleCells = ship->cellsInRange(*ship->getCell(), hexMap, ship->getView(), gl::RangeMode::VIEW);
                for (auto* cell : visibleCells) {
                    if (uniqueCells.find(cell) == uniqueCells.end()) {
                        uniqueCells.insert(cell);
                        vieweableHexes.push_back(cell);
                    }
                }
            }
        }

        // отрисовка карты
        window.clear(sf::Color::Black);
        for (const auto& hexp : seenCells) {
            const auto& hex = *hexp;
            double x_pos = hex.q * hexRadius * 1.5;
            double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
            sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);

            hexShape.setFillColor(getColorByScheme(hex.getNoise(), colSchemeInactive, deepWater, water, land));
            hexShape.setOutlineColor(MAP_COLORS["very_dark_gray"]);
            hexShape.setOutlineThickness(1);
            if (colSchemeInactive == INVERT) {
                hexShape.setOutlineColor(MAP_COLORS["dark_gray"]);
            }

            sf::Sprite goldSprite, shipSprite;
            bool seenGold = false;
            if (std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end()) {
                hexShape.setFillColor(getColorByScheme(hex.getNoise(), colScheme, deepWater, water, land));
                hexShape.setOutlineColor(MAP_COLORS["dark_gray"]);

                // рисуем только те объкты которые видим
                // все что дальше с else if то отрисовывается только тогда когда нет на нем корабля
                if (hex.hasShip()) {
                    switch (hex.getShip()->getOwner()) {
                        case gl::Owner::PIRATE:
                            shipSprite.setTexture(pirate_ship_texture);
                            hexShape.setOutlineColor(sf::Color::Black);
                            break;
                        case gl::Owner::ENEMY:
                            shipSprite.setTexture(enemy_ship_texture);
                            hexShape.setOutlineColor(MAP_COLORS["burgundy"]);
                            break;
                        case gl::Owner::PLAYER:
                            shipSprite.setTexture(player_ship_texture);
                            hexShape.setOutlineColor(sf::Color::Green);
                            break;
                        case gl::Owner::FRIENDLY:
                            hexShape.setOutlineColor(sf::Color::White);
                            break;
                    }
                    if (colScheme == INVERT) {
                        switch (hex.getShip()->getOwner()) {
                            case gl::Owner::PIRATE:
                                hexShape.setFillColor(MAP_COLORS["very_dark_gray"]);
                                break;
                            case gl::Owner::ENEMY:
                                hexShape.setFillColor(MAP_COLORS["burgundy"]);
                                break;
                            case gl::Owner::PLAYER:
                                hexShape.setFillColor(MAP_COLORS["dark_green"]);
                                break;
                            case gl::Owner::FRIENDLY:
                                hexShape.setFillColor(sf::Color::White);
                                break;
                        }
                    }
                } else if (hex.hasTreasure()) {
                    goldSprite.setTexture(treasure_texture);
                } else if (hex.hasGold()) {
                    goldSprite.setTexture(gold_texture);
                }

                // для отрисовки только количества монет если на клетке еще кто-то
                if (hex.hasGold()) {
                    seenGold = true;
                }
                normlaizeSprite(goldSprite, hexRadius, x_pos, y_pos);
                normlaizeSprite(shipSprite, hexRadius, x_pos, y_pos);
            }

            // my click
            if (!selectedShip && selectedHex && hex.q == selectedHex->q && hex.r == selectedHex->r) {
                hexShape.setOutlineColor(MAP_COLORS["deep_yellow"]);
                hexShape.setOutlineThickness(1);
            }
            window.draw(hexShape);
            window.draw(goldSprite);
            if (seenGold) drawResourceText(window, hex, x_pos + 50, y_pos + 50, hexRadius, font, font_size);
            window.draw(shipSprite);
        }

        if (selectedShip) {
            gl::Hex* selectedHex = selectedShip->getCell();
            if (selectedHex) {

                std::vector<gl::Hex*> reachableHexes = selectedShip->cellsInRange(*selectedHex, hexMap, selectedHex->getShip()->getMoveRange(), gl::RangeMode::MOVE);
                for (gl::Hex* reachableHex : reachableHexes) {
                    double x_pos = reachableHex->q * hexRadius * 1.5;
                    double y_pos = reachableHex->r * hexRadius * sqrt(3) + (reachableHex->q % 2) * hexRadius * sqrt(3) / 2.0;

                    sf::ConvexShape reachableShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
                    reachableShape.setFillColor(sf::Color(100, 255, 100, 80)); // Полупрозрачный зеленый
                    reachableShape.setOutlineColor(sf::Color(112, 129, 88, 255));
                    reachableShape.setOutlineThickness(1);
                    window.draw(reachableShape);
                }

                std::vector<gl::Hex*> attackRangeHexes = selectedShip->cellsInRange(*selectedHex, hexMap, selectedHex->getShip()->getDamageRange(), gl::RangeMode::DAMAGE);
                for (gl::Hex* reachableHex : attackRangeHexes) {
                    double x_pos = reachableHex->q * hexRadius * 1.5;
                    double y_pos = reachableHex->r * hexRadius * sqrt(3) + (reachableHex->q % 2) * hexRadius * sqrt(3) / 2.0;

                    sf::ConvexShape reachableShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
                    reachableShape.setFillColor(sf::Color(200, 40, 40, 50)); // Полупрозрачный 
                    window.draw(reachableShape);
                }
            }
        }

        for (const auto& hexp : seenCells) {
            const auto& hex = *hexp;
            if (hex.hasShip() && std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end()) {
                double x_pos = hex.q * hexRadius * 1.5;
                double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
                if (colScheme == COLORS) {
                    drawShipBar(window, hex.getShip(), x_pos + 50, y_pos + 50, hexRadius, font, font_size);
                } else if (colScheme == INVERT) {
                }
            }
        }
        
        for (auto* hex : currentPath) {
            sf::CircleShape marker(hexRadius / 3);
            marker.setFillColor(sf::Color(100, 200, 255, 120));
            marker.setOrigin(marker.getRadius(), marker.getRadius());
            float hx = hex->q * hexRadius * 1.5f + 50;
            float hy = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0f + 50;
            marker.setPosition(hx, hy);
            window.draw(marker);
        }

        window.display();
    }

    return 0;
}
