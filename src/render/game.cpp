#include <SFML/Graphics.hpp>
#include <iostream>
#include "../game/map.h"
namespace gl = GameLogic;

class Game {
private:
    gl::Ship* selectedShip = nullptr;
    
public:
    void handleEvents(sf::RenderWindow& window);
    void drawShips(sf::RenderWindow& window);
};

void Game::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        // ПКМ - выбор корабля и цели
        if (event.type == sf::Event::MouseButtonPressed && 
            event.mouseButton.button == sf::Mouse::Right) {
            
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Mouse::getPosition(window)
            );
            
            // Преобразуем координаты в гекс
            gl::Hex* clickedHex = screenToHex(mousePos);
            if (!clickedHex) continue;
            
            // Если корабль еще не выбран - ищем корабль на этой клетке
            if (!selectedShip) {
                for (auto& ship : playerShips) {
                    if (ship->getCurrentHex() == clickedHex) {
                        selectedShip = ship;
                        std::cout << "Корабль выбран: " << ship->getName() << std::endl;
                        break;
                    }
                }
            } 
            // Если корабль уже выбран - перемещаем его
            else {
                // Проверяем, что целевая клетка - вода
                if (clickedHex->getType() == CellType::SHALLOW_WATER || 
                    clickedHex->getType() == CellType::DEEP_WATER) {
                    
                    // Немедленное перемещение
                    selectedShip->setCurrentHex(clickedHex);
                    std::cout << "Корабль перемещен на: " 
                              << clickedHex->q << ", " << clickedHex->r << std::endl;
                    
                    // Сбрасываем выбор
                    selectedShip = nullptr;
                } else {
                    std::cout << "Нельзя переместиться на сушу!" << std::endl;
                }
            }
        }
        
        // Esc - отмена выбора
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            selectedShip = nullptr;
            std::cout << "Выбор отменен" << std::endl;
        }
    }
}
