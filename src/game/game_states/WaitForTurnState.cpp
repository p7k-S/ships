#include "WaitForTurnState.h"
#include "Game.h"

void WaitForTurnState::enter() {
    // Инициализация состояния ожидания
    // Блокировка интерфейса, показ индикатора ожидания
}

void WaitForTurnState::exit() {
    // Очистка состояния ожидания
    // Скрытие индикатора ожидания
}

void WaitForTurnState::update(float dt) {
    // Обновление логики ожидания
    // Проверка завершения хода противника/ИИ
    // Таймеры, анимации ожидания
    
    // Пример: если ход противника завершен
    if (isOpponentTurnFinished()) {
        Game::getInstance().getStateManager().changeState("MyTurn");
    }
}

void WaitForTurnState::render(sf::RenderWindow& window) {
    // Отрисовка во время ожидания
    // Можно добавить затемнение или индикатор "Ожидание хода противника"
}

void WaitForTurnState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Переход в паузу даже во время ожидания
            Game::getInstance().getStateManager().changeState("Pause");
        }
    }
}

std::string WaitForTurnState::getStateName() const {
    return "WaitForTurn";
}

// Заглушка - нужно реализовать вашу логику
bool WaitForTurnState::isOpponentTurnFinished() {
    return false; // Заменить на реальную проверку
}
