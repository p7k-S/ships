#include "Game.h"
#include "../render/Colors.hpp"
#include "../render/sprites.h"
#include "../render/info_bars.h"

#include "map/GenerateMap.h"
#include "map/Cell.h"
#include "troops/Ship.h"
#include "troops/Soldier.h"

#include "GameLogic.h"
#include "items/Treasure.h"

#include "GameConfig.h"
#include "GameLogic.h"
#include <vector>
#include "../textures/EmbeddedResources.h"
#include <algorithm>


void Game::renderUI() {
    renderSidebar();
    renderBottomBar();
}

void Game::renderSidebar() {
    int windowWidth = window.getSize().x;
    int windowHeight = window.getSize().y;
    
    // Фиксированные размеры (не меняются при ресайзе)
    const int SIDEBAR_WIDTH = 250;
    const int PADDING = 10;
    const int TITLE_FONT_SIZE = 20;
    const int NORMAL_FONT_SIZE = 14;
    const int LINE_HEIGHT = 25;
    
    int sidebarX = windowWidth - SIDEBAR_WIDTH;
    
    sf::RectangleShape sidebar(sf::Vector2f(SIDEBAR_WIDTH, windowHeight));
    sidebar.setPosition(sidebarX, 0);
    sidebar.setFillColor(sf::Color(30, 30, 40));
    window.draw(sidebar);
    
    sf::Text title("CELL INFO", EmbeddedResources::main_font, TITLE_FONT_SIZE);
    title.setPosition(sidebarX + PADDING, 20);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    int yPos = 60;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, view);
    gl::Hex* hoveredHex = selectedHex;
    auto vc = players[p_id]->getViewableCells();
    if (selectedTroop) {
        hoveredHex = selectedTroop->getCell();
    }

    if (hoveredHex && std::find(vc.begin(), vc.end(), hoveredHex) != vc.end()) {
        sf::Text coordText("Coord: (" + std::to_string(hoveredHex->q) + "," + std::to_string(hoveredHex->r) + ")", 
                          EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        coordText.setPosition(sidebarX + PADDING, yPos);
        coordText.setFillColor(sf::Color::White);
        window.draw(coordText);
        yPos += LINE_HEIGHT;

        sf::Text typeText("Type: " + std::to_string(static_cast<int>(hoveredHex->getCellType())), 
                         EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        typeText.setPosition(sidebarX + PADDING, yPos);
        typeText.setFillColor(sf::Color::White);
        window.draw(typeText);
        yPos += LINE_HEIGHT;

        sf::Text goldText("Gold: " + std::to_string(hoveredHex->getGold()), 
                         EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        goldText.setPosition(sidebarX + PADDING, yPos);
        goldText.setFillColor(sf::Color::Yellow);
        window.draw(goldText);
        yPos += LINE_HEIGHT;

        sf::Text noiseText("Noise: " + std::to_string(hoveredHex->getNoise()), 
                          EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        noiseText.setPosition(sidebarX + PADDING, yPos);
        noiseText.setFillColor(sf::Color::White);
        window.draw(noiseText);
        yPos += LINE_HEIGHT;

        if (hoveredHex->hasTroop()) {
            sf::Text troopText("Has Unit", EmbeddedResources::main_font, NORMAL_FONT_SIZE);
            troopText.setPosition(sidebarX + PADDING, yPos);
            troopText.setFillColor(sf::Color::Green);
            window.draw(troopText);
            yPos += LINE_HEIGHT;
        }

        if (hoveredHex->hasBuilding()) {
            sf::Text buildingText("Has Building", EmbeddedResources::main_font, NORMAL_FONT_SIZE);
            buildingText.setPosition(sidebarX + PADDING, yPos);
            buildingText.setFillColor(sf::Color::Magenta);
            window.draw(buildingText);
            yPos += LINE_HEIGHT;
        }

        sf::Text itemsText("Items: " + std::to_string(hoveredHex->getItemsSize()), 
                          EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        itemsText.setPosition(sidebarX + PADDING, yPos);
        itemsText.setFillColor(sf::Color::Cyan);
        window.draw(itemsText);
        yPos += LINE_HEIGHT;
    }

    if (selectedTroop) {
        yPos += 10;
        sf::Text titleTroop("Troop info", EmbeddedResources::main_font, TITLE_FONT_SIZE);
        titleTroop.setPosition(sidebarX + PADDING, yPos);
        titleTroop.setFillColor(sf::Color::White);
        window.draw(titleTroop);
        yPos += 35;

        std::ostringstream infoStream;
        infoStream << "Gold: " << selectedTroop->getGold() 
                  << "/" << selectedTroop->getMaxGold() << "\n"
                  << "Dmg: " << selectedTroop->getDamage() << "\n"
                  << "Hp: " << selectedTroop->getHealth() 
                  << "/" << selectedTroop->getMaxHealth() << "\n"
                  << "View Range: " << (int)selectedTroop->getView() << "\n"
                  << "Move Range: " << (int)selectedTroop->getMoveRange() << "\n"
                  << "Attack Range: " << (int)selectedTroop->getDamageRange() << "\n"
                  << "Item: " << (bool)selectedTroop->hasItem();

        sf::Text troopInfo(infoStream.str(), EmbeddedResources::main_font, NORMAL_FONT_SIZE);
        troopInfo.setPosition(sidebarX + PADDING, yPos);
        troopInfo.setFillColor(sf::Color::White);
        window.draw(troopInfo);
        yPos += 140;
        
        sf::RectangleShape divider(sf::Vector2f(SIDEBAR_WIDTH - PADDING * 2, 2));
        divider.setPosition(sidebarX + PADDING, yPos);
        divider.setFillColor(sf::Color(100, 100, 120));
        window.draw(divider);
        yPos += 15;
        
        renderTroopUpgrades(selectedTroop, sidebarX, SIDEBAR_WIDTH, yPos);
    }
}

void Game::renderTroopUpgrades(gl::Troop* troop, int sidebarX, int sidebarWidth, int& yPos) {
    troopUpgradeButtons.clear();
    if (!troop) return;
    
    const int PADDING = 10;
    const int BUTTON_WIDTH = sidebarWidth - PADDING * 2;
    const int BUTTON_HEIGHT = 30;
    const int SMALL_FONT_SIZE = 12;
    
    sf::Text upgradeTitle("UNIT UPGRADES", EmbeddedResources::main_font, 16);
    upgradeTitle.setPosition(sidebarX + PADDING, yPos);
    upgradeTitle.setFillColor(sf::Color::Yellow);
    window.draw(upgradeTitle);
    yPos += 25;
    
    std::vector<std::tuple<std::string, int, UpgradeType>> upgrades = {
        {"+20 HP", 100, UpgradeType::HEALTH},
        {"+20 DMG", 100, UpgradeType::DAMAGE}, 
        {"+1 View", 50, UpgradeType::VIEW_RANGE},
        {"+1 Attack", 50, UpgradeType::ATTACK_RANGE},
        {"+1 Move", 50, UpgradeType::MOVE_RANGE}
    };
    
    for (const auto& upgrade : upgrades) {
        std::string text = std::get<0>(upgrade);
        int cost = std::get<1>(upgrade);
        UpgradeType type = std::get<2>(upgrade);
        
        if (yPos > window.getSize().y - 100) break;
        
        sf::RectangleShape button(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        button.setPosition(sidebarX + PADDING, yPos);
        button.setFillColor(sf::Color(60, 60, 80));
        button.setOutlineColor(sf::Color(100, 100, 120));
        button.setOutlineThickness(1);
        window.draw(button);
        
        sf::Text buttonText(text + " | " + std::to_string(cost) + "g", 
                           EmbeddedResources::main_font, SMALL_FONT_SIZE);
        buttonText.setPosition(sidebarX + PADDING + 5, yPos + 8);
        buttonText.setFillColor(sf::Color::White);
        window.draw(buttonText);
        
        TroopUpgradeButton upgradeButton;
        upgradeButton.bounds = button.getGlobalBounds();
        upgradeButton.cost = cost;
        upgradeButton.troop = troop;
        upgradeButton.upgradeType = type;
        
        troopUpgradeButtons.push_back(upgradeButton);
        yPos += BUTTON_HEIGHT + 5;
    }

    gl::Hex* troopCell = troop->getCell();
    gl::Hex& cellRef = *troopCell;
    if (troopCell && !troopCell->hasBuilding() && portCanPlayced(cellRef)) {
        if (yPos < window.getSize().y - 100) {
            sf::RectangleShape shipButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
            shipButton.setPosition(sidebarX + PADDING, yPos);
            shipButton.setFillColor(sf::Color(30, 30, 30));
            shipButton.setOutlineColor(sf::Color(100, 100, 120));
            shipButton.setOutlineThickness(1);
            window.draw(shipButton);

            sf::Text shipText("BUY PORT | 500g", EmbeddedResources::main_font, SMALL_FONT_SIZE);
            shipText.setPosition(sidebarX + PADDING + 5, yPos + 8);
            shipText.setFillColor(sf::Color::White);
            window.draw(shipText);

            TroopUpgradeButton shipButtonInfo;
            shipButtonInfo.bounds = shipButton.getGlobalBounds();
            shipButtonInfo.cost = 500;
            shipButtonInfo.troop = troop;
            shipButtonInfo.upgradeType = UpgradeType::BUY_PORT;

            troopUpgradeButtons.push_back(shipButtonInfo);
            yPos += BUTTON_HEIGHT + 5;
        }
    }

    if (troopCell && troopCell->hasBuilding()) {
        auto* building = troopCell->getBuilding();
        if (building && building->isPort() && yPos < window.getSize().y - 100) {
            yPos += 10;
            
            if (yPos < window.getSize().y - 100) {
                sf::RectangleShape soldierButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
                soldierButton.setPosition(sidebarX + PADDING, yPos);
                soldierButton.setFillColor(sf::Color(60, 80, 60));
                soldierButton.setOutlineColor(sf::Color(100, 120, 100));
                soldierButton.setOutlineThickness(1);
                window.draw(soldierButton);

                sf::Text soldierText("BUY SOLDIER | 50g", EmbeddedResources::main_font, SMALL_FONT_SIZE);
                soldierText.setPosition(sidebarX + PADDING + 5, yPos + 8);
                soldierText.setFillColor(sf::Color::White);
                window.draw(soldierText);

                TroopUpgradeButton soldierButtonInfo;
                soldierButtonInfo.bounds = soldierButton.getGlobalBounds();
                soldierButtonInfo.cost = 50;
                soldierButtonInfo.troop = troop;
                soldierButtonInfo.upgradeType = UpgradeType::BUY_SOLDIER;

                troopUpgradeButtons.push_back(soldierButtonInfo);
                yPos += BUTTON_HEIGHT + 5;
            }
            
            if (yPos < window.getSize().y - 100) {
                sf::RectangleShape shipButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
                shipButton.setPosition(sidebarX + PADDING, yPos);
                shipButton.setFillColor(sf::Color(60, 60, 200));
                shipButton.setOutlineColor(sf::Color(100, 100, 120));
                shipButton.setOutlineThickness(1);
                window.draw(shipButton);

                sf::Text shipText("BUY SHIP | 100g", EmbeddedResources::main_font, SMALL_FONT_SIZE);
                shipText.setPosition(sidebarX + PADDING + 5, yPos + 8);
                shipText.setFillColor(sf::Color::White);
                window.draw(shipText);

                TroopUpgradeButton shipButtonInfo;
                shipButtonInfo.bounds = shipButton.getGlobalBounds();
                shipButtonInfo.cost = 100;
                shipButtonInfo.troop = troop;
                shipButtonInfo.upgradeType = UpgradeType::BUY_SHIP;

                troopUpgradeButtons.push_back(shipButtonInfo);
                yPos += BUTTON_HEIGHT + 5;
            }
            
            yPos += 10;

            if (yPos < window.getSize().y - 100) {
                sf::RectangleShape convertButton(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
                convertButton.setPosition(sidebarX + PADDING, yPos);
                convertButton.setFillColor(sf::Color(80, 60, 100));
                convertButton.setOutlineColor(sf::Color(120, 100, 140));
                convertButton.setOutlineThickness(1);
                window.draw(convertButton);

                sf::Text convertText("CONVERT UNIT | 50g", EmbeddedResources::main_font, SMALL_FONT_SIZE);
                convertText.setPosition(sidebarX + PADDING + 5, yPos + 8);
                convertText.setFillColor(sf::Color::White);
                window.draw(convertText);

                TroopUpgradeButton convertButtonInfo;
                convertButtonInfo.bounds = convertButton.getGlobalBounds();
                convertButtonInfo.cost = 50;
                convertButtonInfo.troop = troop;
                convertButtonInfo.upgradeType = UpgradeType::CONVERT;

                troopUpgradeButtons.push_back(convertButtonInfo);
                yPos += BUTTON_HEIGHT + 5;
            }
        }
    }
}

void Game::renderBottomBar() {
    int windowWidth = window.getSize().x;
    int windowHeight = window.getSize().y;
    
    const int BOTTOM_BAR_HEIGHT = 80;
    const int FONT_SIZE = 18;
    const int CONTROLS_FONT_SIZE = 14;
    
    int bottomBarY = windowHeight - BOTTOM_BAR_HEIGHT;
    
    sf::RectangleShape bottomBar(sf::Vector2f(windowWidth, BOTTOM_BAR_HEIGHT));
    bottomBar.setPosition(0, bottomBarY);
    bottomBar.setFillColor(sf::Color(25, 25, 35));
    window.draw(bottomBar);
    
    sf::Text turnText("Turn: " + std::to_string(totalTurnCount), 
                      EmbeddedResources::main_font, FONT_SIZE);
    turnText.setPosition(20, bottomBarY + 20);
    turnText.setFillColor(sf::Color::White);
    window.draw(turnText);
    
    sf::Text playerText("Player: " + players[p_id]->getName(),
                        EmbeddedResources::main_font, FONT_SIZE);
    playerText.setPosition(150, bottomBarY + 20);
    playerText.setFillColor(players[p_id]->getColor());
    window.draw(playerText);

    int exploredPercent = (players[p_id]->getSeenCells().size() * 100) / (mapHeight * mapWidth);
    sf::Text exploredText("Explored: " + std::to_string(exploredPercent) + "%",
                         EmbeddedResources::main_font, FONT_SIZE);
    exploredText.setPosition(350, bottomBarY + 20);
    exploredText.setFillColor(COLORS["deep_yellow"]);
    window.draw(exploredText);

    sf::Text movesLeft("MOVES LEFT: " + std::to_string(getMoveAmount()), 
                       EmbeddedResources::main_font, FONT_SIZE);
    movesLeft.setPosition(550, bottomBarY + 20);
    movesLeft.setFillColor(COLORS["green"]);
    window.draw(movesLeft);
    
    sf::Text controls("G - End Turn | C - Color Scheme", 
                      EmbeddedResources::main_font, CONTROLS_FONT_SIZE);
    controls.setPosition(windowWidth - 400, bottomBarY + 50);
    controls.setFillColor(sf::Color(200, 200, 200));
    window.draw(controls);
}

bool Game::isUIAreaClicked(const sf::Vector2f& mouseUI) {
    int windowWidth = window.getSize().x;
    int windowHeight = window.getSize().y;

    const int SIDEBAR_WIDTH = 250;
    const int BOTTOM_BAR_HEIGHT = 80;

    if (mouseUI.x > windowWidth - SIDEBAR_WIDTH)
        return true;

    if (mouseUI.y > windowHeight - BOTTOM_BAR_HEIGHT)
        return true;

    return false;
}


void Game::handleUIClick(const sf::Vector2f& mouseUI) {
    int windowWidth = window.getSize().x;
    int windowHeight = window.getSize().y;

    const int SIDEBAR_WIDTH = 250;
    const int BOTTOM_BAR_HEIGHT = 80;

    if (mouseUI.x > windowWidth - SIDEBAR_WIDTH) {
        for (const auto& button : troopUpgradeButtons) {
            if (button.bounds.contains(mouseUI)) {
                handleTroopUpgrade(button);
                return;
            }
        }
    }

    // if (mouseUI.y > windowHeight - BOTTOM_BAR_HEIGHT) {
    //     future logic
    // }
}


void Game::handleTroopUpgrade(const TroopUpgradeButton& upgradeButton) {
    gl::Troop* troop = upgradeButton.troop;
    bool cannotUpgrade = false;
    if (!troop) return;

    if (troop->getGold() < upgradeButton.cost) {
        return;
    }

    switch (upgradeButton.upgradeType) {
        case UpgradeType::HEALTH:
            if (troop->getHealth() >= 500) {
                cannotUpgrade = true;
            } else {
                troop->setHealth(troop->getHealth()+20);
            }
            break;
            
        case UpgradeType::DAMAGE:
            if (troop->getDamage() >= 300) {
                cannotUpgrade = true;
            } else {
                troop->setDamage(troop->getDamage()+20);
            }
            break;
            
        case UpgradeType::VIEW_RANGE:
            if (troop->getView() >= 10) {
                cannotUpgrade = true;
            } else {
                troop->setView(troop->getView()+1);
                addViewedCells(players[p_id]->getSeenCells(), troop, hexMap, gl::RangeMode::VIEW);
            }
            break;
            
        case UpgradeType::ATTACK_RANGE:
            if (troop->getView() < troop->getDamageRange()+1) {
                cannotUpgrade = true;
            } else {
                troop->setDamageRange(troop->getDamageRange()+1);
            }
            break;
            
        case UpgradeType::MOVE_RANGE:
            if (troop->getView() < troop->getMoveRange()+1) {
                cannotUpgrade = true;
            } else {
                troop->setMoveRange(troop->getMoveRange()+1);
            }
            break;

        case UpgradeType::BUY_SOLDIER: {
            if (players[p_id]->getBuildings().size()*2 <= players[p_id]->getTroops().size()) {
                cannotUpgrade = true;
            } else {
                placeSoldier(*troop->getCell());
            }
            break;
        }

        case UpgradeType::BUY_SHIP: {
            if (players[p_id]->getBuildings().size()*2 <= players[p_id]->getTroops().size()) {
                cannotUpgrade = true;
            } else {
                placeShip(*troop->getCell());
            }
            break;
        }

        case UpgradeType::BUY_PORT: {
            gl::Owner owner = players[p_id].get();
            auto port = std::make_unique<gl::Port>(owner, troop->getCell());
            gl::Port* portPtr = port.get();
            troop->getCell()->setBuildingOf<gl::Port>(portPtr);
            players[p_id]->addBuilding(std::move(port));
            break;
        }

        case UpgradeType::CONVERT: {
            troop->loseGold(upgradeButton.cost);

            uint16_t gold = troop->getGold();
            auto item = troop->loseItem();
            auto cell = troop->getCell();
            gl::Owner owner = players[p_id].get();
            
            bool wasShip = (troop->getType() == gl::Troop::Type::SHIP);
            
            players[p_id]->removeTroop(troop);
            
            if (wasShip) {
                auto soldier = std::make_unique<gl::Soldier>(owner, &hexMap[cell->q + cell->r * mapWidth]);
                soldier->takeGold(gold);
                soldier->addItem(std::move(item));
                
                cell->removeTroop();
                cell->setTroopOf<gl::Soldier>(soldier.get());
                
                players[p_id]->addTroop(std::move(soldier));
                
            } else {
                auto ship = std::make_unique<gl::Ship>(owner, &hexMap[cell->q + cell->r * mapWidth]);
                ship->takeGold(gold);
                ship->addItem(std::move(item));
                
                cell->removeTroop();
                cell->setTroopOf<gl::Ship>(ship.get());
                
                players[p_id]->addTroop(std::move(ship));
                
            }
            
            selectedTroop = cell->getTroop();
            
            break;
        } 
        default:
            return;
    }
    if (!(upgradeButton.upgradeType == UpgradeType::CONVERT) && !cannotUpgrade) {
        troop->loseGold(upgradeButton.cost);
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 20));

    window.setView(view);
    
    updateVisibleCells();
    renderMap();
    renderShipRange();
    renderUnitBars();

    window.setView(defaultView);
    renderUI();

    window.display();
}

void Game::renderWaitMove() {
    window.setView(window.getDefaultView());
    window.clear(sf::Color(15, 15, 25));
    
    sf::Vector2u windowSize = window.getSize();
    
    sf::RectangleShape background(sf::Vector2f(windowSize.x, windowSize.y));
    background.setFillColor(sf::Color(25, 25, 40, 230));
    window.draw(background);
    
    sf::RectangleShape textArea(sf::Vector2f(windowSize.x * 0.7f, 200));
    textArea.setPosition(windowSize.x * 0.15f, windowSize.y * 0.4f);
    textArea.setFillColor(sf::Color(40, 40, 60, 200));
    textArea.setOutlineThickness(3);
    textArea.setOutlineColor(sf::Color(70, 70, 100));
    window.draw(textArea);
    
    sf::Text text;
    text.setFont(EmbeddedResources::main_font);
    
    std::string displayText = "Next turn: " + players[p_id]->getName() + "\n\n";
    displayText += "Press SPACE to continue";
    text.setString(displayText);
    
    text.setFillColor(sf::Color(255, 215, 0));
    text.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                   textBounds.top + textBounds.height / 2.0f);
    text.setPosition(textArea.getPosition().x + textArea.getSize().x / 2.0f,
                     textArea.getPosition().y + textArea.getSize().y / 2.0f);

    sf::Text shadow = text;
    shadow.setFillColor(sf::Color(0, 0, 0, 150));
    shadow.setPosition(text.getPosition().x + 3, text.getPosition().y + 3);
    
    window.draw(shadow);
    window.draw(text);
    
    static sf::Clock blinkClock;
    float blinkTime = blinkClock.getElapsedTime().asSeconds();
    if (static_cast<int>(blinkTime * 2) % 2 == 0) {
        sf::Text hintText;
        hintText.setFont(EmbeddedResources::main_font);
        hintText.setString("^^^^^");
        hintText.setCharacterSize(24);
        hintText.setFillColor(sf::Color(150, 150, 255, 200));
        
        sf::FloatRect hintBounds = hintText.getLocalBounds();
        hintText.setOrigin(hintBounds.left + hintBounds.width / 2.0f, 0);
        hintText.setPosition(windowSize.x / 2.0f, 
                           textArea.getPosition().y + textArea.getSize().y + 20);
        
        window.draw(hintText);
    }
    
    drawCornerDecorations(window);
    
    window.display();
}

void Game::drawCornerDecorations(sf::RenderWindow& window) {
    sf::Vector2u size = window.getSize();
    sf::RectangleShape corner(sf::Vector2f(30, 30));
    corner.setFillColor(sf::Color::Transparent);
    corner.setOutlineThickness(2);
    corner.setOutlineColor(sf::Color(100, 100, 150, 100));
    
    corner.setPosition(20, 20);
    window.draw(corner);
    
    corner.setPosition(size.x - 50, 20);
    window.draw(corner);
    
    corner.setPosition(20, size.y - 50);
    window.draw(corner);

    corner.setPosition(size.x - 50, size.y - 50);
    window.draw(corner);
}

void Game::renderMap() {
    sf::FloatRect viewBounds = getViewBounds();
    
    for (const auto& hexp : players[p_id]->getSeenCells()) {
        const auto& hex = *hexp;
        double x_pos = hex.q * hexRadius * 1.5;
        double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
        
        if (!isHexInView(x_pos, y_pos, hexRadius, viewBounds)) {
            continue;
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
    return sf::FloatRect(center.x - size.x/2 - 30, center.y - size.y/2 - 30, size.x, size.y);
}

void Game::renderHex(const gl::Hex& hex, float x_pos, float y_pos) {
    sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius - 1);

    std::vector<gl::Hex*> view_cells = players[p_id]->getViewableCells();
    bool isVisible = std::find(view_cells.begin(), view_cells.end(), &hex) != view_cells.end();
    auto currentScheme = isVisible ? colSchemeDefault : colSchemeInactive;

    hexShape.setFillColor(getColorByScheme(hex.getNoise(), currentScheme, deepWater, water, land));
    hexShape.setOutlineColor(COLORS[isVisible ? "dark_gray" : "very_dark_gray"]);
    hexShape.setOutlineThickness(1);

    sf::Sprite sprite;
    bool hasGold = false;
    bool hasBuilding = false;
    bool hasTreasure = false;
    bool hasTroop = false;
    
    if (isVisible) {
        hasTroop = hex.hasTroop();
        hasBuilding = hex.hasBuilding();
        hasTreasure = hex.hasItemOf<gl::Treasure>();
        hasGold = hex.hasGold();
        
        if (hex.hasBuilding()) {
            auto ownerVariant = hex.getBuilding()->getOwner();
            auto color = std::visit([](auto* owner) {
                    return owner->getColor();
                    }, ownerVariant);

            hexShape.setFillColor(color);
        }

        if (hasTroop) {
            gl::Troop* troop = hex.getTroop();
            if (troop && typeid(*troop) == typeid(gl::Ship)) {
                renderShipOnHex(hex, hexShape, sprite);
            }
            else if (troop && typeid(*troop) == typeid(gl::Soldier)) {
                renderSoldierOnHex(hex, hexShape, sprite);
            }
        }
        else if (hasBuilding) {
            sprite.setTexture(EmbeddedResources::port_texture);
        }
        else if (hasTreasure) {
            sprite.setTexture(EmbeddedResources::treasure_texture);
        }
        else if (hasGold) {
            sprite.setTexture(EmbeddedResources::gold_texture);
        }
        normlaizeSprite(sprite, hexRadius, x_pos, y_pos);
    }

    if (!selectedTroop && selectedHex && hex.q == selectedHex->q && hex.r == selectedHex->r) {
        hexShape.setOutlineColor(COLORS["deep_yellow"]);
        hexShape.setOutlineThickness(2);
    }

    window.draw(hexShape);
    
    if (hasGold) {
        drawResourceText(window, hex, x_pos + 50, y_pos + 50, hexRadius, EmbeddedResources::main_font, font_size);
    }
    
    if (isVisible && (hasTroop || hasBuilding || hasTreasure || hasGold)) {
        window.draw(sprite);
    }
}

void Game::renderShipRange() {
    if (!selectedTroop) return;
    
    gl::Hex* selectedHex = selectedTroop->getCell();
    if (!selectedHex) return;

    std::vector<gl::Hex*> reachableHexes = cellsInRange(
        *selectedHex, hexMap, selectedTroop->getMoveRange(), gl::RangeMode::MOVE);
    for (gl::Hex* reachableHex : reachableHexes) {
        renderRangeHex(reachableHex, sf::Color(100, 255, 100, 80), sf::Color(112, 129, 88, 255));
    }

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

void Game::renderSoldierOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite) {
    auto* troop = hex.getTroop();
    if (!troop) return;
    auto troopOwner = troop->getOwner();

    if (isPlayerOwner(troopOwner)) {
        shipSprite.setTexture(EmbeddedResources::player_soldier_texture);
        hexShape.setOutlineColor(std::get<GameLogic::Player*>(troopOwner)->getColor());
    } else if (isEnemyOwner(troopOwner)) {
        shipSprite.setTexture(EmbeddedResources::enemy_soldier_texture);
        hexShape.setOutlineColor(COLORS["burgundy"]);
    }
    
    if (colSchemeDefault == INVERT) {
        if (isEnemyOwner(troopOwner)) {
                    hexShape.setFillColor(COLORS["burgundy"]);
        } else if (isPlayerOwner(troopOwner)) {
                    hexShape.setFillColor(std::get<GameLogic::Player*>(troopOwner)->getColor());
        }
    }
}

void Game::renderShipOnHex(const gl::Hex& hex, sf::ConvexShape& hexShape, sf::Sprite& shipSprite) {
    auto* troop = hex.getTroop();
    if (!troop) return;
    auto troopOwner = troop->getOwner();

    if (isPlayerOwner(troopOwner)) {
        shipSprite.setTexture(EmbeddedResources::player_ship_texture);
        hexShape.setOutlineColor(std::get<GameLogic::Player*>(troopOwner)->getColor());
    } else if (isEnemyOwner(troopOwner)) {
        shipSprite.setTexture(EmbeddedResources::enemy_ship_texture);
        hexShape.setOutlineColor(COLORS["burgundy"]);
    } else if (isPirateOwner(troopOwner)) {
        shipSprite.setTexture(EmbeddedResources::pirate_ship_texture);
        hexShape.setOutlineColor(sf::Color::Black);
    }
    
    if (colSchemeDefault == INVERT) {
        if (isPirateOwner(troopOwner)) {
                    hexShape.setFillColor(COLORS["very_dark_gray"]);
        } else if (isEnemyOwner(troopOwner)) {
                    hexShape.setFillColor(COLORS["burgundy"]);
        } else if (isPlayerOwner(troopOwner)) {
                    hexShape.setFillColor(std::get<GameLogic::Player*>(troopOwner)->getColor());
        }
    }
}

void Game::renderUnitBars() {
    std::vector<gl::Hex*> view_cells = players[p_id]->getViewableCells();
    for (const auto& hexp : players[p_id]->getSeenCells()) {
        const auto& hex = *hexp;
        bool isVisible = std::find(view_cells.begin(), view_cells.end(), hexp) != view_cells.end();
        
        if ((hex.hasTroop() || hex.hasBuilding()) && isVisible) {
            double x_pos = hex.q * hexRadius * 1.5;
            double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;
            
            if (colSchemeDefault == COLORFULL) {
                if (hex.hasTroop() && hex.hasBuilding()) {
                    drawShipBar(window, static_cast<gl::Ship*>(hex.getTroop()), x_pos + 50, y_pos + 50, hexRadius, EmbeddedResources::main_font, font_size);
                    drawPortBar(window, static_cast<gl::Port*>(hex.getBuilding()), x_pos + 50, y_pos + 100, hexRadius, EmbeddedResources::main_font, font_size);
                } else {
                    drawShipBar(window, static_cast<gl::Ship*>(hex.getTroop()), x_pos + 50, y_pos + 50, hexRadius, EmbeddedResources::main_font, font_size);
                    drawPortBar(window, static_cast<gl::Port*>(hex.getBuilding()), x_pos + 50, y_pos + 50, hexRadius, EmbeddedResources::main_font, font_size);
                }
            }
        }
    }
}

void Game::addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Troop* troop, std::vector<gl::Hex>& hexMap, gl::RangeMode mode){
    std::vector<gl::Hex*> newCells = cellsInRange(*troop->getCell(), hexMap, troop->getView(), mode);
    std::unordered_set<gl::Hex*> uniqueSet(seenCells.begin(), seenCells.end());
    seenCells.reserve(seenCells.size() + newCells.size());

    for (auto* cell : newCells) {
        if (uniqueSet.insert(cell).second) {
            seenCells.push_back(cell);
        }
    }
}

void Game::addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Building* building, std::vector<gl::Hex>& hexMap, gl::RangeMode mode){
    std::vector<gl::Hex*> newCells = cellsInRange(*building->getCell(), hexMap, building->getView(), mode);
    std::unordered_set<gl::Hex*> uniqueSet(seenCells.begin(), seenCells.end());
    seenCells.reserve(seenCells.size() + newCells.size());

    for (auto* cell : newCells) {
        if (uniqueSet.insert(cell).second) {
            seenCells.push_back(cell);
        }
    }
}
