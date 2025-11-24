#include "MyTurnState.h"
#include "Game.h"
#include "TroopSelectedState.h"
#include "WaitForTurnState.h"

void MyTurnState::onEnter(Game& game) {
    game.isProcessingTurn = true;
    game.changeTurnLocal = false;
    game.resetSelection();
}

void MyTurnState::handleEvents(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            game.handleKeyPressed(event);
            
            if (event.key.code == sf::Keyboard::Escape) {
                game.changeState(std::make_unique<PauseState>());
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f worldPos = game.window.mapPixelToCoords(
                    sf::Mouse::getPosition(game.window)
                );
                
                // Если уже выбран юнит - обрабатываем как выбор цели
                if (game.selectedTroop) {
                    game.handleTargetSelection(worldPos);
                    // Если действие выполнено, переходим в состояние с выбранным юнитом
                    if (game.targetHex) {
                        game.changeState(std::make_unique<TroopSelectedState>());
                    }
                } else {
                    // Иначе выбираем юнита
                    game.handleTroopSelection(worldPos);
                    if (game.selectedTroop) {
                        game.changeState(std::make_unique<TroopSelectedState>());
                    }
                }
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            game.handleMouseMove(event);
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            game.handleMouseWheel(event);
        }
    }
}

void MyTurnState::update(Game& game) {
    processTurnLogic(game);
    
    // Проверка условий окончания хода
    if (game.changeTurnLocal) {
        if (game.isNetworkGame) {
            game.changeState(std::make_unique<WaitForTurnState>());
        } else {
            // В локальной игре сразу переходим к следующему игроку
            game.nextTurn();
        }
    }
    
    // Проверка условий окончания игры
    if (game.checkGameOver()) {
        game.changeState(std::make_unique<GameOverState>());
    }
}

void MyTurnState::processTurnLogic(Game& game) {
    if (game.isProcessingTurn) {
        game.isProcessingTurn = false;
        game.p_id = game.nextAlivePlayer();
        
        if (game.isNetworkGame) {
            game.sendTurnUpdate();
        }
    }
    
    game.updateVisibleCells();
    game.cleanupDestroyedShips();
}

void MyTurnState::render(Game& game) {
    game.render();
}

void MyTurnState::onExit(Game& game) {
    game.resetSelection();
}
