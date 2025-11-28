#include "GameOverState.h"
#include "Game.h"

void GameOverState::enter() {
    // Инициализация экрана завершения игры
    // Показ результатов, кнопки рестарта/выхода
}

void GameOverState::exit() {
    // Очистка экрана завершения игры
}

void GameOverState::update(float dt) {
    // Обновление экрана завершения (анимации)
}

void GameOverState::render(sf::RenderWindow& window) {
    // Отрисовка экрана завершения игры
}

void GameOverState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            // Рестарт игры - переход к главному меню или началу игры
            Game::getInstance().getStateManager().changeState("MainMenu");
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            // Выход в главное меню
            Game::getInstance().getStateManager().changeState("MainMenu");
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        // Обработка кликов по кнопкам (Рестарт, Выход и т.д.)
    }
}

std::string GameOverState::getStateName() const {
    return "GameOver";
}
