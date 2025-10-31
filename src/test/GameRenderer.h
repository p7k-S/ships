#pragma once
#include <SFML/Graphics.hpp>
#include "../game/GameLogic.h"

namespace GameRender {
    class GameRenderer {
    private:
        sf::RenderWindow& window;
        sf::Font font;
        float hexSize;
        
    public:
        GameRenderer(sf::RenderWindow& win, float hexSize = 40.0f) 
            : window(win), hexSize(hexSize) {
            // Попытка загрузить шрифт
            if (!font.loadFromFile("assets/fonts/pixel.ttf")) {
                std::cout << "Шрифт не загружен, используется стандартный\n";
            }
        }
        
        sf::Vector2f hexToPixel(const GameLogic::HexCoord& hex) {
            float x = hexSize * 1.5f * hex.q;
            float y = hexSize * std::sqrt(3.0f) * (hex.r + 0.5f * (hex.q % 2));
            return sf::Vector2f(x + 400, y + 300);
        }
        
        void drawCell(const GameLogic::Cell& cell, const GameLogic::HexCoord& coord) {
            sf::Vector2f position = hexToPixel(coord);
            
            // Простой шестиугольник через круг
            sf::CircleShape hexagon(hexSize, 6);
            hexagon.setPosition(position);
            hexagon.setOrigin(hexSize, hexSize);
            hexagon.setRotation(30);
            
            // Цвет в зависимости от типа клетки
            sf::Color color;
            if (!cell.discovered) {
                color = sf::Color::Black;
            } else if (!cell.visible) {
                color = sf::Color(50, 50, 50);
            } else {
                switch (cell.type) {
                    case GameLogic::CellType::OCEAN: color = sf::Color(0, 100, 200); break;
                    case GameLogic::CellType::ISLAND: color = sf::Color(139, 69, 19); break;
                    case GameLogic::CellType::PIRATES: color = sf::Color::Red; break;
                    case GameLogic::CellType::TREASURE: color = sf::Color::Yellow; break;
                    case GameLogic::CellType::PORT: color = sf::Color::Green; break;
                }
            }
            
            hexagon.setFillColor(color);
            hexagon.setOutlineColor(sf::Color::White);
            hexagon.setOutlineThickness(1);
            window.draw(hexagon);
        }
        
        void drawShip(const GameLogic::Ship& ship) {
            sf::Vector2f position = hexToPixel(ship.position);
            sf::CircleShape shipShape(hexSize/2);
            shipShape.setPosition(position);
            shipShape.setOrigin(hexSize/2, hexSize/2);
            shipShape.setFillColor(sf::Color::White);
            window.draw(shipShape);
        }
        
        void drawHUD(const GameLogic::Ship& ship, GameLogic::Weather weather) {
            // Панель ресурсов
            sf::RectangleShape hudBackground(sf::Vector2f(250, 150));
            hudBackground.setPosition(10, 10);
            hudBackground.setFillColor(sf::Color(0, 0, 0, 128));
            window.draw(hudBackground);
            
            // Текст с информацией
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(16);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(20, 20);
            
            std::string weatherStr;
            switch (weather) {
                case GameLogic::Weather::CALM: weatherStr = "Штиль"; break;
                case GameLogic::Weather::FOG: weatherStr = "Туман"; break;
                case GameLogic::Weather::STORM: weatherStr = "Шторм"; break;
                case GameLogic::Weather::FROST: weatherStr = "Мороз"; break;
            }
            
            infoText.setString(
                "Здоровье: " + std::to_string(ship.health) + "\n" +
                "Припасы: " + std::to_string(ship.supplies) + "\n" +
                "Золото: " + std::to_string(ship.gold) + "\n" +
                "Клады: " + std::to_string(ship.treasuresFound) + "/3\n" +
                "Погода: " + weatherStr + "\n" +
                "Управление: Space - ход, R - отдых"
            );
            window.draw(infoText);
        }
        
        void render(const GameLogic::Game& game) {
            window.clear(sf::Color(30, 30, 60));
            
            // Отрисовка карты
            const auto& map = game.getMap();
            for (int r = 0; r < map.getHeight(); r++) {
                for (int q = 0; q < map.getWidth(); q++) {
                    GameLogic::HexCoord coord(q, r);
                    drawCell(map.getCell(coord), coord);
                }
            }
            
            // Отрисовка корабля
            drawShip(game.getShip());
            
            // Отрисовка HUD
            drawHUD(game.getShip(), game.getWeather());
            
            window.display();
        }
    };
}
