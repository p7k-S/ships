#include "TroopSelectedState.h"
#include "Game.h"
#include "MyTurnState.h"

void TroopSelectedState::onEnter(Game& game) {
    // Можно добавить визуальную обратную связь о выбранном юните
}

void TroopSelectedState::handleEvents(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                // Отмена выбора - возврат в обычное состояние хода
                game.resetSelection();
                game.changeState(std::make_unique<MyTurnState>());
            }
            else if (event.key.code == sf::Keyboard::Space) {
                // Выполнить действие
                executeActionAndReturn(game);
            }
            else {
                game.handleKeyPressed(event);
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f worldPos = game.window.mapPixelToCoords(
                    sf::Mouse::getPosition(game.window)
                );
                
                // Выбор новой цели или отмена
                game.handleTargetSelection(worldPos);
                
                // Если действие выполнено или отменено
                if (game.targetHex == nullptr || game.executeTroopAction()) {
                    executeActionAndReturn(game);
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // ПКМ - отмена выбора
                game.resetSelection();
                game.changeState(std::make_unique<MyTurnState>());
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            game.handleMouseMove(event);
        }
    }
}

void TroopSelectedState::executeActionAndReturn(Game& game) {
    // Выполняем действие если есть цель
    if (game.targetHex) {
        game.executeTroopAction();
    }
    
    // Возвращаемся в основное состояние хода
    game.changeState(std::make_unique<MyTurnState>());
}

void TroopSelectedState::update(Game& game) {
    game.updateVisibleCells();
}

void TroopSelectedState::render(Game& game) {
    game.render();
    // Дополнительная отрисовка для выбранного юнита
    game.renderShipRange();
    game.renderPath();
    game.renderShipUI();
}
