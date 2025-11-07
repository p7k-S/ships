#include "Game.h"
// #include "../render/Colors.hpp"
#include "GameConfig.h"


void Game::handleMouseButtonPressed(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left) {
        isDragging = true;
        lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        
        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        if (waitingForMove && selectedShip) {
            handleTargetSelection(worldPos);
        } else {
            handleShipSelection(worldPos);
        }
    }
}
void Game::handleMouseWheel(const sf::Event& event) {
    // Получаем положение курсора в окне
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

    // Преобразуем его в мировые координаты ДО масштабирования
    sf::Vector2f beforeZoom = window.mapPixelToCoords(pixelPos, view);

    // Применяем масштаб
    float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
    view.zoom(zoomFactor);

    // Преобразуем координаты курсора снова, уже после масштабирования
    sf::Vector2f afterZoom = window.mapPixelToCoords(pixelPos, view);

    // Сдвигаем вид так, чтобы точка под курсором осталась на месте
    sf::Vector2f offset = beforeZoom - afterZoom;
    view.move(offset);

    // Применяем изменённый вид
    window.setView(view);
}

void Game::handleMouseMove(const sf::Event& event) {
    if (isDragging) {
        sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        sf::Vector2f delta = lastMousePos - currentMousePos;
        view.move(delta);


        window.setView(view);
        lastMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                handleKeyPressed(event);
                break;

            case sf::Event::MouseButtonPressed:
                handleMouseButtonPressed(event);
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left)
                    isDragging = false;  // 🔥 отпускаем "палец"
                break;

            case sf::Event::MouseWheelScrolled:
                handleMouseWheel(event);
                break;

            case sf::Event::MouseMoved:
                handleMouseMove(event);
                break;

            default:
                break;
        }
    }
}

void Game::handleKeyPressed(const sf::Event& event) {
    if (event.key.code == sf::Keyboard::C) {
        colSchemeDefault = (colSchemeDefault == COLORFULL) ? INVERT : COLORFULL;
        colSchemeInactive = (colSchemeInactive == DARK_COLORS) ? INVERT : DARK_COLORS;
    }

    if (event.key.code == sf::Keyboard::Enter) {
        if (waitingForMove && selectedShip && targetHex)
            executeShipAction();
    }

    if (event.key.code == sf::Keyboard::F) {
        fullscreenMapMode = !fullscreenMapMode;
        // if (fullscreenMapMode) {
        //     mapView.setViewport({0.f, 0.f, 1.f, 1.f});
        // } else {
        //     mapView.setViewport(mapViewport);
        // }
    }

    handleCameraControl(event);
}

void Game::handleCameraControl(const sf::Event& event) {
    if (event.key.code == sf::Keyboard::Left) view.move(-10.f, 0.f);
    if (event.key.code == sf::Keyboard::Right) view.move(10.f, 0.f);
    if (event.key.code == sf::Keyboard::Up) view.move(0.f, -10.f);
    if (event.key.code == sf::Keyboard::Down) view.move(0.f, 10.f);
    if (event.key.code == sf::Keyboard::Equal) view.zoom(0.9f);
    if (event.key.code == sf::Keyboard::Hyphen) view.zoom(1.1f);
    if (event.key.code == sf::Keyboard::R) view = defaultView;

    window.setView(view);
}
