#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
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
    
public:
    SimpleConfigUI() 
        : window(sf::VideoMode(500, 400), "Game Configuration")
    {
        font.loadFromMemory(airborne_ttf,
                airborne_ttf_len);
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
            }

            render();
        }
        return startGame;
    }

private:
    void handleKeyPress(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Space:
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
                playersAmount = std::min(playersAmount, static_cast<uint8_t>(255)); // Ограничение до 4 игроков
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

        float centerX = window.getSize().x / 2.0f;
        float startY = 60.f;
        float lineHeight = 35.f;

        sf::Text title;
        title.setFont(font);
        title.setString("GAME CONFIGURATION");
        title.setCharacterSize(24);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setPosition(centerX - titleBounds.width / 2, 15.f);
        window.draw(title);

        for (size_t i = 0; i < paramNames.size(); ++i) {
            float yPos = startY + i * lineHeight;

            if (i == selectedParam) {
                sf::Text arrow;
                arrow.setFont(font);
                arrow.setString(">");
                arrow.setCharacterSize(20);
                arrow.setFillColor(sf::Color::Yellow);
                arrow.setPosition(centerX - 180, yPos);
                window.draw(arrow);
            }

            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(paramNames[i] + ":");
            nameText.setCharacterSize(18);
            nameText.setFillColor(i == selectedParam ? sf::Color::Yellow : sf::Color(200, 200, 200));
            nameText.setPosition(centerX - 150, yPos);
            window.draw(nameText);

            sf::Text valueText;
            valueText.setFont(font);
            valueText.setString(getParamValue(i));
            valueText.setCharacterSize(18);
            valueText.setFillColor(i == selectedParam ? sf::Color::Yellow : sf::Color::White);
            if (i == selectedParam) {
                valueText.setStyle(sf::Text::Bold);
            }
            
            if (i == 6 && i == selectedParam) {
                valueText.setString("< " + getParamValue(i) + " >");
            }
            
            valueText.setPosition(centerX + 50, yPos);
            window.draw(valueText);
        }

        sf::Text instructions;
        instructions.setFont(font);
        instructions.setString("UP/DOWN: Select   LEFT/RIGHT: Change   R: Random Seed\nSPACE: Start Game");
        instructions.setCharacterSize(14);
        instructions.setFillColor(sf::Color(150, 150, 200));
        sf::FloatRect instrBounds = instructions.getLocalBounds();
        instructions.setPosition(centerX - instrBounds.width / 2, 450.f);
        window.draw(instructions);

        window.display();
    }
};
