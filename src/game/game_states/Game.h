#pragma once
#include <SFML/Graphics.hpp>
#include "GameStateManager.h"

class Game {
private:
    static Game* instance;
    sf::RenderWindow window;
    GameStateManager stateManager;
    
    Game(); // Приватный конструктор для Singleton

public:
    static Game& getInstance();
    
    void run();
    void initialize();
    
    GameStateManager& getStateManager();
    sf::RenderWindow& getWindow();
    
private:
    void handleEvents();
    void update();
    void render();
};
