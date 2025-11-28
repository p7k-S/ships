#include "MainMenuState.h"
#include "../Game.h" // Предполагаем, что у вас есть класс Game для доступа к менеджеру состояний

void MainMenuState::enter() {
    // Инициализация главного меню
    // Загрузка текстур, создание кнопок, настройка UI
}

void MainMenuState::exit() {
    // Очистка ресурсов главного меню
}

void MainMenuState::update(float dt) {
    // Обновление логики меню (анимации, таймеры и т.д.)
}

void MainMenuState::render(sf::RenderWindow& window) {
    // Отрисовка главного меню
    // window.draw(...);
}

void MainMenuState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            // Начать игру - переход к ходу игрока
            Game::getInstance().getStateManager().changeState("MyTurn");
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            // Выход из игры
            window.close();
        }
    }
    
    // Обработка кликов мыши по кнопкам меню
    if (event.type == sf::Event::MouseButtonPressed) {
        // Проверка кликов по кнопкам и переходы между состояниями
    }
}

std::string MainMenuState::getStateName() const {
    return "MainMenu";
}
