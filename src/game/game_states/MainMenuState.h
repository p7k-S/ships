#pragma once
#include "GameState.h"

class MainMenuState : public GameState {
private:
    sf::Texture backgroundTexture;
    sf::Sprite background;
    std::vector<sf::Text> menuItems;
    int selectedItem = 0;
    
public:
    void handleEvents(Game& game) override;
    void update(Game& game, float deltaTime) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
};
