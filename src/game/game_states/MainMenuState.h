#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>

class MainMenuState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    std::string getName() const override { return "MainMenu"; }

private:
    sf::Text titleText;
    sf::Text startLocalText;
    sf::Text startNetworkText;
    sf::Text exitText;
    sf::Font font;
    
    void initializeUI();
    void handleMenuSelection(Game& game, const sf::Vector2f& mousePos);
};
