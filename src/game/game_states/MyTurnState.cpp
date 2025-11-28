#include "MyTurnState.h"
#include "../Game.h"

void MyTurnState::enter() {
    // Активация UI для хода игрока
    // Разблокировка интерфейса, подсветка доступных действий
}

void MyTurnState::exit() {
    // Деактивация UI хода игрока
    // Сброс выделений, блокировка интерфейса
}

void MyTurnState::update(float dt) {
    // Обновление логики хода игрока
    // Анимации, таймеры, проверка условий завершения хода
}

void MyTurnState::render(sf::RenderWindow& window) {
    // Отрисовка игрового поля во время хода игрока
    // Отрисовка юнитов, UI, выделений и т.д.
}

void MyTurnState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Переход в паузу
            Game::getInstance().getStateManager().changeState("Pause");
        }
        else if (event.key.code == sf::Keyboard::Space) {
            // Завершить ход - переход к ожиданию
            Game::getInstance().getStateManager().changeState("WaitForTurn");
        }
    }
    
    // Обработка выбора и перемещения юнитов
    if (event.type == sf::Event::MouseButtonPressed) {
        // Логика выбора юнитов, клеток и т.д.
    }
}

std::string MyTurnState::getStateName() const {
    return "MyTurn";
}
