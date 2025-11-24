#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>

class WaitForTurnState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    std::string getName() const override { return "WaitForTurn"; }

private:
    sf::Text waitText;
    sf::Font font;
};
