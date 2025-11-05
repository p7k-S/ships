#pragma once
#include "GameState.h"
#include "../render/RenderEngine.h"

class Game {
private:
    sf::RenderWindow window;
    GameState gameState;
    RenderEngine renderEngine;
    AssetManager assetManager;
    
    void handleEvents();
    void update();
    void render();
    
public:
    Game();
    void run();
    void initialize();
};
