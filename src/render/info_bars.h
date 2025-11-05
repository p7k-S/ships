#include <SFML/Graphics.hpp>
#include "../game/GameLogic.h"
#include "../game/troops/Ship.h"

namespace gl = GameLogic;

inline void drawShipBar(sf::RenderWindow& window, gl::Ship* ship, float x, float y, float hexRadius, sf::Font& font, int font_size) {
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
        // Функция для рисования текста с обводкой
        auto drawTextWithOutline = [&](const std::string& str, float posX, float posY, sf::Color fillColor, int charSize) {
            // Основной текст
            sf::Text text;
            text.setFont(font);
            text.setString(str);
            text.setCharacterSize(charSize);
            text.setFillColor(fillColor);
            text.setStyle(sf::Text::Bold);
            text.setPosition(posX, posY);
            
            // Черная обводка (рисуем смещенные копии)
            sf::Text outline = text;
            outline.setFillColor(sf::Color::Black);
            
            // Рисуем обводку со смещениями
            outline.setPosition(posX - 1, posY - 1); window.draw(outline);
            outline.setPosition(posX + 1, posY - 1); window.draw(outline);
            outline.setPosition(posX - 1, posY + 1); window.draw(outline);
            outline.setPosition(posX + 1, posY + 1); window.draw(outline);
            
            // Рисуем основной текст поверх
            window.draw(text);
        };
        
        // HP зеленый
        sf::Text hpText;
        hpText.setFont(font);
        hpText.setString(std::to_string(ship->getHealth()));
        hpText.setCharacterSize(font_size);
        hpText.setStyle(sf::Text::Bold);
        
        // Урон красный
        sf::Text damageText;
        damageText.setFont(font);
        damageText.setString(std::to_string(static_cast<int>(ship->getDamage())));
        damageText.setCharacterSize(font_size);
        damageText.setStyle(sf::Text::Bold);
        
        // Позиционируем рядом
        sf::FloatRect hpBounds = hpText.getLocalBounds();
        sf::FloatRect damageBounds = damageText.getLocalBounds();
        
        float totalWidth = hpBounds.width + damageBounds.width + 10; // +10 для отступа
        
        float hpX = x - totalWidth/2;
        float damageX = x - totalWidth/2 + hpBounds.width + 10;
        float textY = y - hexRadius - 20;
        
        // Рисуем с обводкой
        drawTextWithOutline(std::to_string(ship->getHealth()), hpX, textY, sf::Color::Green, 10);
        drawTextWithOutline(std::to_string(static_cast<int>(ship->getDamage())), damageX, textY, sf::Color::Red, 10);
        
        // Золото желтый (добавляем под полоской HP)
        sf::Text goldText;
        goldText.setFont(font);
        goldText.setString(std::to_string(ship->getGold()) + "/" + std::to_string(ship->getMaxGold()));
        goldText.setCharacterSize(font_size);
        goldText.setStyle(sf::Text::Bold);
        
        sf::FloatRect goldBounds = goldText.getLocalBounds();
        float goldX = x - goldBounds.width/2;
        float goldY = y - hexRadius + 2;
        
        // Рисуем золото с обводкой
        drawTextWithOutline(std::to_string(ship->getGold()) + "/" + std::to_string(ship->getMaxGold()), 
                           goldX, goldY, sf::Color::Yellow, 8);
    }
}

inline void drawResourceText(sf::RenderWindow& window, const gl::Hex& hex, float x, float y, float hexRadius, sf::Font& font, int font_size) {
    if (font.getInfo().family == "") return;
    
    sf::Text resourceText;
    resourceText.setFont(font);
    resourceText.setCharacterSize(font_size);
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

inline void addViewedCells(std::vector<gl::Hex*>& seenCells, gl::Ship* ship, std::vector<gl::Hex>& hexMap, gl::RangeMode mode){
    std::vector<gl::Hex*> newCells = ship->cellsInRange(*ship->getCell(), hexMap, ship->getView(), mode);
    std::unordered_set<gl::Hex*> uniqueSet(seenCells.begin(), seenCells.end());
    seenCells.reserve(seenCells.size() + newCells.size());

    for (auto* cell : newCells) {
        if (uniqueSet.insert(cell).second) {
            seenCells.push_back(cell);
        }
    }
}
