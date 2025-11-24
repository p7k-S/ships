#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>

class PauseState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    std::string getName() const override { return "Pause"; }

private:
    sf::Text pauseText;
    sf::Text resumeText;
    sf::Text menuText;
    sf::Font font;
    std::string previousState;
};
