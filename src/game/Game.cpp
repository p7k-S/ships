#include "Game.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include "constants.h"
#include "map/Cell.h"
#include "map/GenerateMap.h"
#include "troops/Ship.h"
#include "../render/Colors.hpp"
#include "../render/ui/startUI.h"
#include "../render/info_bars.h"
#include "../render/sprites.h"

Game::Game() {
}

void Game::run() {
    if (!initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return;
    }
    
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

bool Game::startMenu() {
    SimpleConfigUI configUI;
    if (configUI.show()) {
    }
    return true;
}

bool Game::initialize() {
    if (!startMenu()) {
        return false;
    }


    generateMap();
    distributeCellTypes();
    createShips();
    placeGoldAndTreasures();

    // Создание окна
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "Hex Map", sf::Style::Default, settings);
    view = window.getDefaultView();
    defaultView = view;

    // Загрузка текстур
    if (!loadTextures()) {
        return false;
    }

    // Загрузка шрифта
    if (!font.loadFromFile(font_path)) {
        std::cerr << "Не удалось загрузить шрифт из: " << font_path << std::endl;
        return false;
    }
    
    return true;
}

bool Game::loadTextures() {
    if (!player_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/player_ship.png")) {
        return false;
    }
    if (!pirate_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/pirates_ship.png")) {
        return false;
    }
    if (!enemy_ship_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/enemy_ship.png")) {
        return false;
    }
    if (!gold_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/gold_cons.png")) {
        return false;
    }
    if (!treasure_texture.loadFromFile("/home/zane/Study/mirea/sem3/kursach/src/textures/treasure.png")) {
        return false;
    }
    return true;
}

void Game::generateMap() {
    PerlinNoise perlin(seed);
    hexMap = createMap(perlin, mapWidth, mapHeight, octaves, scale);
}

void Game::distributeCellTypes() {
    // Определение уровней высот для типов клеток
    std::vector<double> noiseValues = getNoises(hexMap);
    std::vector<double> sortedValues = noiseValues;
    std::sort(sortedValues.begin(), sortedValues.end());

    deepWater = sortedValues[noiseValues.size() * deepWater_delim];
    water = sortedValues[noiseValues.size() * water_delim];
    land = sortedValues[noiseValues.size() * land_delim];

    // Распределение клеток по типам
    for (auto& hex : hexMap) {
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
}

void Game::createShips() {
    std::srand(std::time(nullptr));

    for (int i = 0; i < (waterHexes.size() + deepWaterHexes.size()) * percent_ships_in_water; ++i) {
        gl::Owner owner = gl::Owner::PIRATE;
        if (i == 0) {
            owner = gl::Owner::PLAYER;
        } else if (i % 2) {
            owner = gl::Owner::ENEMY;
        }

        int randomIndex = std::rand() % waterHexes.size();
        gl::Hex* startHex = waterHexes[randomIndex];

        // Проверка, что на клетке нет другого корабля
        bool exists = false;
        for (const auto& ship : ships) {
            if (ship->getCell() == startHex) { 
                exists = true; 
                break; 
            }
        }
        if (exists) { 
            --i; 
            continue; 
        }

        // Создаем корабль
        auto myShip = std::make_unique<gl::Ship>(owner, startHex);
        startHex->setShip(myShip.get());
        ships.push_back(std::move(myShip));
    }

    // Добавление видимых клеток для первого корабля игрока
    if (!ships.empty()) {
        addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);
    }
}


void Game::placeGoldAndTreasures() {
    std::uniform_int_distribution<> goldDist(10, 100);
    std::uniform_real_distribution<> chanceDist(0.0, 1.0);

    // Размещение золота
    for (auto& hex : hexMap) {
        if (chanceDist(gen) < 0.15) {
            hex.setGold(goldDist(gen));
        }
    }

    // Размещение сокровищ
    int treasuresToPlace = treasures;
    std::uniform_int_distribution<> indexDist(0, hexMap.size() - 1);
    std::unordered_set<int> usedIndices;

    while (treasuresToPlace > 0 && usedIndices.size() < hexMap.size()) {
        int randomIndex = indexDist(gen);
        if (usedIndices.insert(randomIndex).second) {
            hexMap[randomIndex].setTreasure("nice");
            treasuresToPlace--;
        }
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
                
            case sf::Event::KeyPressed:
                handleKeyPressed(event);
                break;
                
            case sf::Event::MouseButtonPressed:
                handleMouseButtonPressed(event);
                break;
                
            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                }
                break;
                
            case sf::Event::MouseMoved:
                handleMouseMove(event);
                break;
                
            case sf::Event::MouseWheelScrolled:
                handleMouseWheel(event);
                break;
            case sf::Event::Resized:
            case sf::Event::LostFocus:
            case sf::Event::GainedFocus:
            case sf::Event::TextEntered:
            case sf::Event::KeyReleased:
            case sf::Event::MouseWheelMoved:
            case sf::Event::MouseEntered:
            case sf::Event::MouseLeft:
            case sf::Event::JoystickButtonPressed:
            case sf::Event::JoystickButtonReleased:
            case sf::Event::JoystickMoved:
            case sf::Event::JoystickConnected:
            case sf::Event::JoystickDisconnected:
            case sf::Event::TouchBegan:
            case sf::Event::TouchMoved:
            case sf::Event::TouchEnded:
            case sf::Event::SensorChanged:
            case sf::Event::Count:
              break;
            }
    }
}

void Game::handleKeyPressed(const sf::Event& event) {
    // Смена цветовой схемы
    if (event.key.code == sf::Keyboard::C) {
        colScheme = (colScheme == COLORS) ? INVERT : COLORS;
        colSchemeInactive = (colSchemeInactive == DARK_COLORS) ? INVERT : DARK_COLORS;
    }

    // Подтверждение действия корабля
    if (event.key.code == sf::Keyboard::Enter) {
        if (waitingForMove && selectedShip && targetHex) {
            executeShipAction();
        }
    }

    // Управление камерой
    handleCameraControl(event);
}

void Game::handleCameraControl(const sf::Event& event) {
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

void Game::handleMouseButtonPressed(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left) {
        isDragging = true;
        lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        
        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        if (waitingForMove && selectedShip) {
            handleTargetSelection(worldPos);
        } else {
            handleShipSelection(worldPos);
        }
    }
}

void Game::handleMouseMove(const sf::Event& event) {
    if (isDragging) {
        sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        sf::Vector2f delta = lastMousePos - currentMousePos;
        view.move(delta);
        window.setView(view);
        lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    }
}

void Game::handleMouseWheel(const sf::Event& event) {
    if (event.mouseWheelScroll.delta > 0) {
        view.zoom(0.9f);
    } else {
        view.zoom(1.1f);
    }
    window.setView(view);
}

void Game::handleShipSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
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
            break;
        }
    }
}

void Game::handleTargetSelection(const sf::Vector2f& worldPos) {
    for (auto& hex : hexMap) {
        if (isPointInHex(worldPos, hex)) {
            targetHex = &hex;
            std::vector<gl::Hex*> reachableHexes = selectedShip->cellsInRange(
                *selectedShip->getCell(), hexMap, selectedShip->getMoveRange(), gl::RangeMode::MOVE);
            // currentPath = gl::Ship::getShortestRoad(reachableHexes, selectedShip->getCell(), targetHex);

            if (targetHex->getShip() && targetHex->getShip()->getOwner() != gl::Owner::FRIENDLY) {
                std::cout << "Цель выбрана: атака вражеского корабля на (" 
                    << targetHex->q << ", " << targetHex->r << ")" << std::endl;
            } else {
                std::cout << "Цель выбрана: перемещение на (" 
                    << targetHex->q << ", " << targetHex->r << ")" << std::endl;
            }
            std::cout << "Нажмите Enter для подтверждения действия или выберите другую цель" << std::endl;
            break;
        }
    }
}

void Game::executeShipAction() {
    bool actionPerformed = false;

    // Перемещение
    if (selectedShip->canMoveTo(targetHex, hexMap)) {
        selectedShip->moveTo(targetHex, hexMap);
        selectedShip->takeGoldFromCell(targetHex);
        addViewedCells(seenCells, &*ships[0], hexMap, gl::RangeMode::VIEW);
        std::cout << "Корабль перемещен на (" << targetHex->q << ", " << targetHex->r << ")" << std::endl;
        actionPerformed = true;
    }
    // Атака
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
        resetSelection();
    } else {
        std::cout << "Невозможно выполнить действие" << std::endl;
    }
}

bool Game::isPointInHex(const sf::Vector2f& point, const gl::Hex& hex) {
    float hx = hex.q * hexRadius * 1.5 + 50;
    float hy = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0 + 50;
    float dx = point.x - hx;
    float dy = point.y - hy;
    return std::sqrt(dx*dx + dy*dy) <= hexRadius;
}

void Game::update() {
    cleanupDestroyedShips();
    updateVisibleCells();
}

void Game::cleanupDestroyedShips() {
    for (auto& ship : ships) {
        if (ship->isDestroyed()) {
            ship->Destroy();
        }
    }
    ships.erase(
        std::remove_if(ships.begin(), ships.end(),
            [](const std::unique_ptr<gl::Ship>& s) { return s->isDestroyed(); }),
        ships.end());
}

void Game::updateVisibleCells() {
    vieweableHexes.clear();
    std::unordered_set<gl::Hex*> uniqueCells;
    
    for (auto& ship : ships) {
        if (ship->getOwner() == gl::Owner::PLAYER) {
            std::vector<gl::Hex*> visibleCells = ship->cellsInRange(
                *ship->getCell(), hexMap, ship->getView(), gl::RangeMode::VIEW);
            for (auto* cell : visibleCells) {
                if (uniqueCells.find(cell) == uniqueCells.end()) {
                    uniqueCells.insert(cell);
                    vieweableHexes.push_back(cell);
                }
            }
        }
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    renderMap();
    renderShipRange();
    renderPath();
    renderShipUI();
    
    window.display();
}

void Game::renderMap() {
    for (const auto& hexp : seenCells) {
        const auto& hex = *hexp;
        double x_pos = hex.q * hexRadius * 1.5;
        double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
        
        renderHex(hex, x_pos, y_pos);
    }
}

void Game::renderHex(const gl::Hex& hex, float x_pos, float y_pos) {
    sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);

    // Определяем видимость и цветовую схему
    bool isVisible = std::find(vieweableHexes.begin(), vieweableHexes.end(), &hex) != vieweableHexes.end();
    auto currentScheme = isVisible ? colScheme : colSchemeInactive;

    hexShape.setFillColor(getColorByScheme(hex.getNoise(), currentScheme, deepWater, water, land));
    hexShape.setOutlineColor(MAP_COLORS[isVisible ? "dark_gray" : "very_dark_gray"]);
    hexShape.setOutlineThickness(1);

    sf::Sprite goldSprite, shipSprite;
    bool seenGold = false;
    
    if (isVisible) {
        // Отрисовка объектов на видимых гексах
        if (hex.hasShip()) {
            renderShipOnHex(hex, hexShape, shipSprite);
        } else if (hex.hasTreasure()) {
            goldSprite.setTexture(treasure_texture);
        } else if (hex.hasGold()) {
            goldSprite.setTexture(gold_texture);
            seenGold = true;
        }
        
        normlaizeSprite(goldSprite, hexRadius, x_pos, y_pos);
        normlaizeSprite(shipSprite, hexRadius, x_pos, y_pos);
    }

    // Выделение выбранного гекса
    if (!selectedShip && selectedHex && hex.q == selectedHex->q && hex.r == selectedHex->r) {
        hexShape.setOutlineColor(MAP_COLORS["deep_yellow"]);
        hexShape.setOutlineThickness(2);
    }

    window.draw(hexShape);
    window.draw(goldSprite);
    
    if (seenGold) {
        drawResourceText(window, hex, x_pos + 50, y_pos + 50, hexRadius, font, font_size);
    }
    
    window.draw(shipSprite);
}

void Game::renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite) {
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
}

void Game::renderShipRange() {
    if (!selectedShip) return;
    
    gl::Hex* selectedHex = selectedShip->getCell();
    if (!selectedHex) return;

    // Диапазон перемещения
    std::vector<gl::Hex*> reachableHexes = selectedShip->cellsInRange(
        *selectedHex, hexMap, selectedShip->getMoveRange(), gl::RangeMode::MOVE);
    for (gl::Hex* reachableHex : reachableHexes) {
        renderRangeHex(reachableHex, sf::Color(100, 255, 100, 80), sf::Color(112, 129, 88, 255));
    }

    // Диапазон атаки
    std::vector<gl::Hex*> attackRangeHexes = selectedShip->cellsInRange(
        *selectedHex, hexMap, selectedShip->getDamageRange(), gl::RangeMode::DAMAGE);
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

void Game::renderShipUI() {
    for (const auto& hexp : seenCells) {
        const auto& hex = *hexp;
        if (hex.hasShip() && std::find(vieweableHexes.begin(), vieweableHexes.end(), hexp) != vieweableHexes.end()) {
            double x_pos = hex.q * hexRadius * 1.5;
            double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
            if (colScheme == COLORS) {
                drawShipBar(window, hex.getShip(), x_pos + 50, y_pos + 50, hexRadius, font, font_size);
            }
        }
    }
}

void Game::resetSelection() {
    waitingForMove = false;
    selectedShip = nullptr;
    selectedHex = nullptr;
    targetHex = nullptr;
    currentPath.clear();
}

void Game::cleanup() {
    ships.clear();
    seenCells.clear();
    vieweableHexes.clear();
    currentPath.clear();
}
