#pragma once
#include "GameState.h"

class TroopSelectedState : public GameState {
public:
    void handleEvents(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;
    void onEnter(Game& game) override;
    std::string getName() const override { return "TroopSelected"; }

private:
    void executeActionAndReturn(Game& game);
};
