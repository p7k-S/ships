#include "PauseState.h"
#include "Game.h"

void PauseState::enter() {
    // Инициализация меню паузы
    // Затемнение игры, показ меню паузы
}

void PauseState::exit() {
    // Восстановление игрового состояния
    // Уборка меню паузы
}

void PauseState::update(float dt) {
    // Обновление меню паузы (анимации)
}

void PauseState::render(sf::RenderWindow& window) {
    // Сначала отрисовываем предыдущее состояние (игру)
    // Затем поверх - меню паузы
}

void PauseState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Возврат к предыдущему состоянию
            Game::getInstance().getStateManager().returnToPreviousState();
        }
        else if (event.key.code == sf::Keyboard::Q) {
            // Выход в главное меню
            Game::getInstance().getStateManager().changeState("MainMenu");
        }
        else if (event.key.code == sf::Keyboard::R) {
            // Возобновление игры
            Game::getInstance().getStateManager().returnToPreviousState();
        }
    }
}

std::string PauseState::getStateName() const {
    return "Pause";
}
