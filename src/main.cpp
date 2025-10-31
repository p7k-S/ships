#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
// #include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>
#include <iostream>
#include "game/map.h"
#include "game/ship.h"
#include "game/detect_islands.h"
// #include "game/islands_utils.h"
#include "render/Colors.hpp"
// #include "render/map/CubePerlin.h"
#include "render/map/MapReneder.h"

namespace gl = GameLogic;

void drawShipBar(sf::RenderWindow& window, gl::Ship* ship, float x, float y, float hexRadius, sf::Font& font) {
    if (!ship) return;
    
    float healthPercent = static_cast<float>(ship->getHealth()) / ship->getMaxHealth();
    float barWidth = hexRadius * 1.2f;
    float barHeight = 4.0f;
    
    // Полоска HP
    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setPosition(x - barWidth/2, y - hexRadius - 8);
    background.setFillColor(sf::Color::Black);
    window.draw(background);
    
    sf::RectangleShape healthBar(sf::Vector2f(barWidth * healthPercent, barHeight));
    healthBar.setPosition(x - barWidth/2, y - hexRadius - 8);
    healthBar.setFillColor(healthPercent > 0.5f ? sf::Color::Green : sf::Color::Red);
    window.draw(healthBar);
    
    if (font.getInfo().family != "") {
        // HP зеленый
        sf::Text hpText;
        hpText.setFont(font);
        hpText.setString(std::to_string(ship->getHealth()));
        hpText.setCharacterSize(10);
        hpText.setFillColor(sf::Color::Green);
        hpText.setStyle(sf::Text::Bold);
        
        // Урон красный
        sf::Text damageText;
        damageText.setFont(font);
        damageText.setString(std::to_string(static_cast<int>(ship->getDamage())));
        damageText.setCharacterSize(10);
        damageText.setFillColor(sf::Color::Red);
        damageText.setStyle(sf::Text::Bold);
        
        // Позиционируем рядом
        sf::FloatRect hpBounds = hpText.getLocalBounds();
        sf::FloatRect damageBounds = damageText.getLocalBounds();
        
        float totalWidth = hpBounds.width + damageBounds.width + 10; // +10 для отступа
        
        hpText.setPosition(x - totalWidth/2, y - hexRadius - 20);
        damageText.setPosition(x - totalWidth/2 + hpBounds.width + 10, y - hexRadius - 20);
        
        window.draw(hpText);
        window.draw(damageText);
                // Золото желтый (добавляем под полоской HP)
        sf::Text goldText;
        goldText.setFont(font);
        goldText.setString(std::to_string(ship->getGold()) + "/" + std::to_string(ship->getMaxGold()));
        goldText.setCharacterSize(8);
        goldText.setFillColor(sf::Color::Yellow);
        goldText.setStyle(sf::Text::Bold);
        
        sf::FloatRect goldBounds = goldText.getLocalBounds();
        goldText.setPosition(x - goldBounds.width/2, y - hexRadius + 2);
        
        window.draw(goldText);
    }
}

void drawResourceText(sf::RenderWindow& window, const gl::Hex& hex, float x, float y, float hexRadius, sf::Font& font) {
    if (font.getInfo().family == "") return;
    
    sf::Text resourceText;
    resourceText.setFont(font);
    resourceText.setCharacterSize(10);
    resourceText.setFillColor(sf::Color::Yellow);
    resourceText.setStyle(sf::Text::Bold);
    
    std::string text;
    if (hex.hasGold()) {
        // text = std::to_string(hex.getGold()) + "G"; // Предполагая, что есть метод getGoldAmount()
        text = std::to_string(hex.getGold()); // Предполагая, что есть метод getGoldAmount()
    } 
    // else if (hex.hasTreasure()) {
    //     text = std::to_string(hex.getGold()) + "T"; // Предполагая, что есть метод getTreasureAmount()
    // }
    
    resourceText.setString(text);
    
    // Центрируем текст под спрайтом золота
    sf::FloatRect textBounds = resourceText.getLocalBounds();
    resourceText.setPosition(x - textBounds.width/2, y + hexRadius/2);
    
    // Добавляем черную обводку для лучшей читаемости
    sf::Text outlineText = resourceText;
    outlineText.setFillColor(sf::Color::Black);
    outlineText.setPosition(x - textBounds.width/2 + 1, y + hexRadius/2 + 1);
    window.draw(outlineText);
    
    window.draw(resourceText);
}

void addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Ship* ship, std::vector<gl::Hex>& hexMap, bool isView){
    std::vector<gl::Hex*> newCells = ship->getCellsInRadius(*ship->getCell(), hexMap, ship->getView(), isView);
    std::unordered_set<gl::Hex*> uniqueSet(seenCells.begin(), seenCells.end());
    seenCells.reserve(seenCells.size() + newCells.size());

    for (auto* cell : newCells) {
        if (uniqueSet.insert(cell).second) {
            seenCells.push_back(cell);
        }
    }
}
void normlaizeSprite(sf::Sprite&sprite, const double hexRadius, double x_pos,double y_pos){
    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    float scale = (hexRadius * 1.0f) / spriteBounds.width; // Меньше чем клетка
    sprite.setScale(scale, scale);
    sprite.setPosition(
            x_pos + 50 - spriteBounds.width * scale / 2,
            y_pos + 50 - spriteBounds.height * scale / 2
            );
}

int main() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    const int mapWidth = 25;
    const int mapHeight = 15;
    const double multipl = 0.1;         // опытным путем, можно и захардкодить
    const double persistance = 0.5;     // опытным путем, можно и захардкодить
    const int seed = 1, octaves = 2;
    const bool random_map = false; // сид на рандом
    const bool unknown_map = false; // отрисовка карты
    const double hexRadius = 30.0;  // размер карты
    const double deepWater_delim = 0.2;
    const double water_delim = 0.65;
    const double land_delim = 0.9;

    sf::Font font;
    // Загрузка шрифта
    if (!font.loadFromFile("/home/zane/.fonts/JetBrains/JetBrainsMonoNerdFont-Light.ttf")) {
        std::cerr << "Не удалось загрузить шрифт!" << std::endl;
        // Обработка ошибки
    }

    PerlinNoise perlin(seed);
    std::vector<gl::Hex> hexMap = createMap(perlin, mapWidth, mapHeight, octaves);

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

    std::vector<std::vector<gl::Hex*>> islands = gl::findIslands(hexMap, mapWidth, mapHeight);
    // после надо проставить островам овнеров
            // hex.setOwner(gl::Owner::PLAYER);


    // if (size(islands) < 3) {
    //     return 0;
    // }
    std::vector player_island = islands.front();
    std::vector enemy_island = islands.back();
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
    addViewedCells(seenCells, &*ships[0], hexMap, true);

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
    // sf::RenderWindow window(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default, settings);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default);
    sf::View view = window.getDefaultView(); // Get the default view

    gl::Hex* selectedHex = nullptr;
    gl::Ship* selectedShip = nullptr;
    bool waitingForMove = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                colScheme = (colScheme == COLORS) ? INVERT : COLORS;
            }

            // if (event.type == sf::Event::MouseWheelScrolled) {
            //     if (event.mouseWheelScroll.delta > 0) {
            //         view.zoom(0.9f); // Zoom in
            //     } else {
            //         view.zoom(1.1f); // Zoom out
            //     }
            //     window.setView(view); // Apply the new view
            // }

            // if (event.type == sf::Event::KeyPressed)
            // {
            //     if (event.key.code == sf::Keyboard::Left)
            //         view.move(-10.f, 0.f); // Move left
            //     if (event.key.code == sf::Keyboard::Right)
            //         view.move(10.f, 0.f);  // Move right
            //     if (event.key.code == sf::Keyboard::Up)
            //         view.move(0.f, -10.f); // Move up
            //     if (event.key.code == sf::Keyboard::Down)
            //         view.move(0.f, 10.f);  // Move down
            //     window.setView(view); // Apply the updated view
            // }

            // Обработка клика мыши
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                for (auto& hex : hexMap) {
                    float hx = hex.q * hexRadius * 1.5 + 50;
                    float hy = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0 + 50;
                    // Проверка попадания (аппроксимация через круг)
                    float dx = mousePos.x - hx;
                    float dy = mousePos.y - hy;

                    if (std::sqrt(dx*dx + dy*dy) <= hexRadius) {
                        if (waitingForMove) {
                            // --- Перемещение выбранного корабля ---
                            if (selectedShip && selectedShip->canMoveTo(&hex, hexMap)) {
                                selectedShip->moveTo(&hex, hexMap);
                                selectedShip->takeGoldFromCell(&hex); // если хочешь собрать то будь добр остановитсься на ней
                                addViewedCells(seenCells, &*ships[0], hexMap, true);
                                std::cout << "Корабль перемещен на (" << hex.q << ", " << hex.r << ")" << std::endl;
                            }
                            // --- Атака вражеского корабля на соседней клетке ---
                            else if (selectedShip && hex.getShip() && 
                                    hex.getShip()->getOwner() != gl::Owner::FRIENDLY &&
                                    gl::Ship::areNeighbors(selectedShip->getCell(), &hex)) {
                                selectedShip->giveDamage(&hex);
                                std::cout << "Атакован вражеский корабль на (" << hex.q << ", " << hex.r << ")" << 
                                    "       hp: " << hex.getShip()->getHealth() << "     my hp " << selectedShip->getHealth() << std::endl;

                                if (hex.getShip()->isDestroyed()) {
                                    std::cout << "Корабль противника уничтожен!" << std::endl;
                                }
                            }
                            else {
                                // std::cout << "Невозможно переместить корабль сюда" << std::endl;
                            }


                            waitingForMove = false;
                            selectedShip = nullptr;
                            selectedHex = nullptr;

                        } else {
                            if (hex.getShip() && hex.getShip()->getOwner() == gl::Owner::PLAYER) {
                                selectedShip = hex.getShip();
                                waitingForMove = true;
                                std::cout << "Корабль выбран. Выберите цель для перемещения" << std::endl;
                            } else {
                                // Нет корабля - просто подсвечиваем клетку
                                selectedHex = &hex;
                                selectedShip = nullptr;
                                waitingForMove = false;
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
                std::vector<gl::Hex*> visibleCells = ship->getCellsInRadius(*ship->getCell(), hexMap, ship->getView(), true);
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

            hexShape.setFillColor(getColorByScheme(hex.getNoise(), INVERT, deepWater, water, land));
            hexShape.setOutlineColor(MAP_COLORS[1]);
            hexShape.setOutlineThickness(1);

            sf::Sprite goldSprite, shipSprite;
            bool seenGold = false;
            if (std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end()) {
                hexShape.setFillColor(getColorByScheme(hex.getNoise(), colScheme, deepWater, water, land));

                //рисуем только те объкты которые видим
                if (hex.hasShip()) {
                    switch (hex.getShip()->getOwner()) {
                        case gl::Owner::PIRATE:
                            shipSprite.setTexture(pirate_ship_texture);
                            hexShape.setOutlineColor(sf::Color::Black);
                            break;
                        case gl::Owner::ENEMY:
                            shipSprite.setTexture(enemy_ship_texture);
                            hexShape.setOutlineColor(MAP_COLORS[3]);
                            break;
                        case gl::Owner::PLAYER:
                            shipSprite.setTexture(player_ship_texture);
                            hexShape.setOutlineColor(sf::Color::Green);
                            break;
                        case gl::Owner::FRIENDLY:
                            hexShape.setOutlineColor(MAP_COLORS[6]);
                            break;
                    }
                } else if (hex.hasGold()) {
                    goldSprite.setTexture(gold_texture);
                    seenGold = true;
                } else if (hex.hasTreasure()) {
                    goldSprite.setTexture(treasure_texture);
                }
                normlaizeSprite(goldSprite, hexRadius, x_pos, y_pos);
                normlaizeSprite(shipSprite, hexRadius, x_pos, y_pos);
            }

            // my click
            if (selectedHex && hex.q == selectedHex->q && hex.r == selectedHex->r) {
                hexShape.setOutlineColor(MAP_COLORS[2]);
                hexShape.setOutlineThickness(1);
            }
            window.draw(hexShape);
            window.draw(goldSprite);
            if (seenGold) drawResourceText(window, hex, x_pos + 50, y_pos + 50, hexRadius, font);
            window.draw(shipSprite);
        }
        if (selectedShip) {
            gl::Hex* selectedHex = selectedShip->getCell();
            if (selectedHex) {
                std::vector<gl::Hex*> reachableHexes = selectedShip->getCellsInRadius(*selectedHex, hexMap, selectedHex->getShip()->getMoveRange(), false);

                for (gl::Hex* reachableHex : reachableHexes) {
                    double x_pos = reachableHex->q * hexRadius * 1.5;
                    double y_pos = reachableHex->r * hexRadius * sqrt(3) + (reachableHex->q % 2) * hexRadius * sqrt(3) / 2.0;

                    sf::ConvexShape reachableShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
                    reachableShape.setFillColor(sf::Color(100, 255, 100, 20)); // Полупрозрачный зеленый
                    reachableShape.setOutlineColor(sf::Color::Yellow);
                    reachableShape.setOutlineThickness(1);
                    window.draw(reachableShape);
                }
            }
        }

        for (const auto& hexp : seenCells) {
            const auto& hex = *hexp;
            if (hex.hasShip() && std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end() && colScheme == COLORS) {
                double x_pos = hex.q * hexRadius * 1.5;
                double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
                drawShipBar(window, hex.getShip(), x_pos + 50, y_pos + 50, hexRadius, font);
            }
        }

        //отрисовка кораблей
        // for (const auto& ship : ships) {
        //     // Получаем гекс, на котором находится корабль
        //     gl::Hex* hex = ship->getCell();
        //     if (!hex) continue; // Пропускаем если гекс не установлен
        //
        //     // Вычисляем позицию гекса
        //     double x_pos = hex->q * hexRadius * 1.5;
        //     double y_pos = hex->r * hexRadius * sqrt(3) + (hex->q % 2) * hexRadius * sqrt(3) / 2.0;
        //     sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);
        //     hexShape.setFillColor(getColorByScheme(hex->getNoise(), colScheme, deepWater, water, land));
        //
        //     sf::Sprite shipSprite;
        //     switch (hex->getShip()->getOwner()) {
        //         case gl::Owner::PIRATE:
        //             shipSprite.setTexture(pirate_ship_texture);
        //             hexShape.setOutlineColor(sf::Color::Black);
        //             break;
        //         case gl::Owner::ENEMY:
        //             shipSprite.setTexture(enemy_ship_texture);
        //             hexShape.setOutlineColor(MAP_COLORS[3]);
        //             break;
        //         case gl::Owner::PLAYER:
        //             shipSprite.setTexture(player_ship_texture);
        //             hexShape.setOutlineColor(sf::Color::Green);
        //             break;
        //         case gl::Owner::FRIENDLY:
        //             hexShape.setOutlineColor(MAP_COLORS[6]);
        //             break;
        //     }
        //     hexShape.setOutlineThickness(1);
        //     window.draw(hexShape);
        //
        //     // Создаем спрайт корабля (не клетки!)
        //
        //     // Настройка размера
        //     sf::FloatRect spriteBounds = shipSprite.getLocalBounds();
        //     float scale = (hexRadius * 1.0f) / spriteBounds.width; // Меньше чем клетка
        //
        //     shipSprite.setScale(scale, scale);
        //
        //     // Позиционирование по центру шестиугольника
        //     shipSprite.setPosition(
        //             x_pos + 50 - spriteBounds.width * scale / 2,
        //             y_pos + 50 - spriteBounds.height * scale / 2
        //             );
        //
        //
        //     window.draw(shipSprite);
        // }
            // if (hex.hasGold()) {
            //     sf::Sprite goldSprite;
            //     goldSprite.setTexture(gold_texture);
            //     sf::FloatRect spriteBounds = goldSprite.getLocalBounds();
            //     float scale = (hexRadius * 1.0f) / spriteBounds.width; // Меньше чем клетка
            //
            //     goldSprite.setScale(scale, scale);
            //
            //     // Позиционирование по центру шестиугольника
            //     goldSprite.setPosition(
            //             x_pos + 50 - spriteBounds.width * scale / 2,
            //             y_pos + 50 - spriteBounds.height * scale / 2
            //             );
            //     window.draw(goldSprite);
            // }

        window.display();
    }

    return 0;
}
