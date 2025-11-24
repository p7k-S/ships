#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>

class GameOverState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    std::string getName() const override { return "GameOver"; }

private:
    sf::Text gameOverText;
    sf::Text resultText;
    sf::Text restartText;
    sf::Text menuText;
    sf::Font font;
};
