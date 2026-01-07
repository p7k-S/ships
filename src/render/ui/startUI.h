#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "../../game/GameConfig.h"
#include "../../textures/UIfont.h"

class SimpleConfigUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    bool startGame = false;
    int selectedParam = 0;
    std::vector<std::string> paramNames = {
        "Map Width", "Map Height", "Scale", "Persistance", 
        "Seed", "Octaves", "Players amount"
    };
    
    // Конфигурация интерфейса
    struct UIConfig {
        float titleFontSize = 24.0f;
        float paramFontSize = 18.0f;
        float instructionFontSize = 14.0f;
        float lineHeight = 35.0f;
        float startY = 60.0f; // Начальная позиция по Y (будет пересчитана)
        float horizontalMargin = 20.0f; // Отступ от краев
        float spacing = 50.0f; // Расстояние между названием параметра и значением
    } uiConfig;
    
public:
    SimpleConfigUI() 
        : window(sf::VideoMode(500, 400), "Game Configuration", sf::Style::Titlebar | sf::Style::Close)
    {
        font.loadFromMemory(airborne_ttf, airborne_ttf_len);
    }

    bool show() {
        while (window.isOpen() && !startGame) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    handleKeyPress(event.key.code);
                }
                
                // Обработка изменения размера окна
                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
            }

            render();
        }
        return startGame;
    }

private:
    void handleKeyPress(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Space:
            case sf::Keyboard::Enter:
                window.close();
                startGame = true;
                break;
                
            case sf::Keyboard::Up:
                selectedParam = (selectedParam - 1 + paramNames.size()) % paramNames.size();
                break;
                
            case sf::Keyboard::Down:
                selectedParam = (selectedParam + 1) % paramNames.size();
                break;
                
            case sf::Keyboard::Left:
                adjustParam(-1);
                break;
                
            case sf::Keyboard::Right:
                adjustParam(1);
                break;
                
            case sf::Keyboard::R:
                seed = std::rand();
                break;
                
            case sf::Keyboard::Escape:
                window.close();
                break;
                
            default:
                break;
        }
    }
    
    void adjustParam(int direction) {
        switch (selectedParam) {
            case 0: // Map Width
                mapWidth = std::max(1, mapWidth + direction);
                break;
            case 1: // Map Height
                mapHeight = std::max(1, mapHeight + direction);
                break;
            case 2: // Scale
                scale = std::max(0.01, scale + direction * 0.01);
                scale = std::round(scale * 100) / 100;
                break;
            case 3: // Persistance
                persistance = std::max(0.0, std::min(1.0, persistance + direction * 0.05));
                persistance = std::round(persistance * 100) / 100;
                break;
            case 4: // Seed
                seed += direction;
                break;
            case 5: // Octaves
                octaves = std::max(1, octaves + direction);
                break;
            case 6: // Players amount
                playersAmount = std::max(1, playersAmount + direction);
                playersAmount = std::min(playersAmount, static_cast<uint8_t>(255));
                break;
            default:
                break;
        }
    }
    
    std::string getParamValue(int index) {
        switch (index) {
            case 0: return std::to_string(mapWidth);
            case 1: return std::to_string(mapHeight);
            case 2: return std::to_string(scale).substr(0, 4);
            case 3: return std::to_string(persistance).substr(0, 4);
            case 4: return std::to_string(seed);
            case 5: return std::to_string(octaves);
            case 6: return std::to_string(static_cast<int>(playersAmount));
            default: return "";
        }
    }

    void render() {
        window.clear(sf::Color(40, 40, 60));

        // Получаем размеры окна
        sf::Vector2u windowSize = window.getSize();
        float centerX = windowSize.x / 2.0f;

        // Динамически рассчитываем позиции элементов
        float titleTopMargin = 20.0f;
        float lineHeight = uiConfig.lineHeight * (windowSize.y / 400.0f);
        float startY = windowSize.y * 0.15f;

        // Рендерим заголовок
        sf::Text title;
        title.setFont(font);
        title.setString("GAME CONFIGURATION");
        title.setCharacterSize(static_cast<unsigned int>(uiConfig.titleFontSize * (windowSize.y / 400.0f)));
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);

        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setPosition(centerX - titleBounds.width / 2, titleTopMargin);
        window.draw(title);

        // Рассчитываем максимальную ширину для центрирования блока
        float maxTotalWidth = 0;
        float maxNameWidth = 0;
        float maxValueWidth = 0;

        for (size_t i = 0; i < paramNames.size(); ++i) {
            sf::Text tempText;
            tempText.setFont(font);

            // Ширина названия параметра
            tempText.setString(paramNames[i] + ":");
            tempText.setCharacterSize(static_cast<unsigned int>(uiConfig.paramFontSize * (windowSize.y / 400.0f)));
            float nameWidth = tempText.getLocalBounds().width;
            maxNameWidth = std::max(maxNameWidth, nameWidth);

            // Ширина значения параметра
            std::string valueStr = getParamValue(i);
            if (i == 6 && i == selectedParam) {
                valueStr = "< " + valueStr + " >";
            }
            tempText.setString(valueStr);
            float valueWidth = tempText.getLocalBounds().width;
            maxValueWidth = std::max(maxValueWidth, valueWidth);

            // Общая ширина строки
            maxTotalWidth = std::max(maxTotalWidth, nameWidth + uiConfig.spacing + valueWidth);
        }

        // Ограничиваем максимальную ширину
        float availableWidth = windowSize.x - 2 * uiConfig.horizontalMargin - 30.0f; // -30 для стрелки
        maxTotalWidth = std::min(maxTotalWidth, availableWidth);

        // Начальная позиция блока
        float blockStartX = centerX - maxTotalWidth / 2;

        // Рендерим список параметров
        for (size_t i = 0; i < paramNames.size(); ++i) {
            float yPos = startY + i * lineHeight;

            // Стрелка выбора (слева от блока)
            if (i == selectedParam) {
                sf::Text arrow;
                arrow.setFont(font);
                arrow.setString(">");
                arrow.setCharacterSize(static_cast<unsigned int>(uiConfig.paramFontSize * (windowSize.y / 400.0f)));
                arrow.setFillColor(sf::Color::Yellow);

                float arrowX = blockStartX - 30.0f; // 30px слева от блока
                arrow.setPosition(arrowX, yPos);
                window.draw(arrow);
            }

            // Название параметра (левая часть)
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(paramNames[i] + ":");
            nameText.setCharacterSize(static_cast<unsigned int>(uiConfig.paramFontSize * (windowSize.y / 400.0f)));
            nameText.setFillColor(i == selectedParam ? sf::Color::Yellow : sf::Color(200, 200, 200));

            // Выравниваем названия по левому краю блока
            nameText.setPosition(blockStartX, yPos);
            window.draw(nameText);

            // Значение параметра (правая часть)
            sf::Text valueText;
            valueText.setFont(font);

            std::string valueStr = getParamValue(i);
            if (i == 6 && i == selectedParam) {
                valueStr = "< " + valueStr + " >";
            }
            valueText.setString(valueStr);
            valueText.setCharacterSize(static_cast<unsigned int>(uiConfig.paramFontSize * (windowSize.y / 400.0f)));
            valueText.setFillColor(i == selectedParam ? sf::Color::Yellow : sf::Color::White);

            if (i == selectedParam) {
                valueText.setStyle(sf::Text::Bold);
            }

            // Вычисляем позицию для выравнивания по правому краю блока
            sf::FloatRect valueBounds = valueText.getLocalBounds();
            float valueX = blockStartX + maxTotalWidth - valueBounds.width;
            valueText.setPosition(valueX, yPos);
            window.draw(valueText);
        }

        // Рендерим инструкции
        sf::Text instructions;
        instructions.setFont(font);
        instructions.setString("UP/DOWN: Select   LEFT/RIGHT: Change   R: Random Seed\nSPACE/ENTER: Start Game   ESC: Exit");
        instructions.setCharacterSize(static_cast<unsigned int>(uiConfig.instructionFontSize * (windowSize.y / 400.0f)));
        instructions.setFillColor(sf::Color(150, 150, 200));
        instructions.setLineSpacing(1.5f);

        sf::FloatRect instrBounds = instructions.getLocalBounds();
        float instrY = windowSize.y - instrBounds.height - 20.0f;
        instructions.setPosition(centerX - instrBounds.width / 2, instrY);
        window.draw(instructions);

        window.display();
    }
};
