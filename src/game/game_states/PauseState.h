#pragma once
#include "GameState.h"

class PauseState : public GameState {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void handleInput(const sf::Event& event) override;
    
    std::string getStateName() const override;
};
