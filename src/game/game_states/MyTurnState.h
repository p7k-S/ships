#pragma once
#include "GameState.h"

class MyTurnState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    std::string getName() const override { return "MyTurn"; }

private:
    void processTurnLogic(Game& game);
};
