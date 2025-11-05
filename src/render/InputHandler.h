#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../game/GameLogic.h"
namespace gl = GameLogic;

class InputHandler {
public:
    static void handleGameInput(sf::Event& event,
                               sf::RenderWindow& window,
                               sf::View& view,
                               sf::View& defaultView,
                               gl::Ship*& selectedShip,
                               bool& waitingForMove,
                               gl::Hex*& targetHex,
                               std::vector<gl::Hex*>& currentPath,
                               bool& isDragging,
                               sf::Vector2f& lastMousePos,
                               std::vector<gl::Hex>& hexMap,
                               std::vector<std::unique_ptr<gl::Ship>>& ships);
    
    static void handleCameraInput(sf::Event& event,
                                 sf::View& view,
                                 sf::View& defaultView,
                                 sf::RenderWindow& window);
    
    static void handleMouseInput(sf::Event& event,
                                sf::RenderWindow& window,
                                bool& isDragging,
                                sf::Vector2f& lastMousePos,
                                sf::View& view,
                                gl::Ship*& selectedShip,
                                bool& waitingForMove,
                                gl::Hex*& targetHex,
                                std::vector<gl::Hex*>& currentPath,
                                std::vector<gl::Hex>& hexMap);
};

#endif
