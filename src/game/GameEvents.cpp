#include "Game.h"
#include "GameConfig.h"

void Game::handleMouseButtonPressed(const sf::Event& event) {
    if (event.mouseButton.button != sf::Mouse::Left)
        return;

    sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);

    sf::Vector2f uiMousePos = window.mapPixelToCoords(pixelPos, defaultView);

    if (isUIAreaClicked(uiMousePos)) {
        handleUIClick(uiMousePos);
        return;
    }

    isDragging = true;

    lastMousePos = window.mapPixelToCoords(pixelPos, view);

    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);

    if (waitingForMove && selectedTroop) {
        handleTargetSelection(worldPos);
    } else {
        handleTroopSelection(worldPos);
    }
}


void Game::handleMouseWheel(const sf::Event& event) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

    sf::Vector2f beforeZoom = window.mapPixelToCoords(pixelPos, view);

    float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
    view.zoom(zoomFactor);

    sf::Vector2f afterZoom = window.mapPixelToCoords(pixelPos, view);

    sf::Vector2f offset = beforeZoom - afterZoom;
    view.move(offset);

    window.setView(view);
}

void Game::handleMouseMove(const sf::Event& event) {
    if (isDragging) {
        sf::Vector2f currentMousePos = window.mapPixelToCoords(
            sf::Vector2i(event.mouseMove.x, event.mouseMove.y), 
            view
        );
        
        sf::Vector2f delta = lastMousePos - currentMousePos;
        view.move(delta);

        window.setView(view);
        
        lastMousePos = window.mapPixelToCoords(
            sf::Vector2i(event.mouseMove.x, event.mouseMove.y), 
            view
        );
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::Resized:
                Rerender = true;
                handleWindowResize(event);
                break;

            case sf::Event::KeyPressed:
                Rerender = true;
                handleKeyPressed(event);
                break;

            case sf::Event::MouseButtonPressed:
                Rerender = true;
                handleMouseButtonPressed(event);
                break;

            case sf::Event::MouseButtonReleased:
                Rerender = true;
                if (event.mouseButton.button == sf::Mouse::Left)
                    isDragging = false;
                break;

            case sf::Event::MouseWheelScrolled:
                Rerender = true;
                handleMouseWheel(event);
                break;

            case sf::Event::MouseMoved:
                Rerender = true;
                handleMouseMove(event);
                break;

            default:
                break;
        }
    }
}

void Game::handleWindowResize(const sf::Event& event) {
    float newWidth = static_cast<float>(event.size.width);
    float newHeight = static_cast<float>(event.size.height);
    
    sf::Vector2f oldCenter = view.getCenter();
    
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    view.setSize(newWidth, newHeight);
    view.setCenter(oldCenter);
    
    defaultView = sf::View(sf::FloatRect(0.f, 0.f, newWidth, newHeight));
    
    window.setView(view);
}

void Game::handleKeyPressed(const sf::Event& event) {
    if (event.key.code == sf::Keyboard::C) {
        colSchemeDefault = (colSchemeDefault == COLORFULL) ? INVERT : COLORFULL;
        colSchemeInactive = (colSchemeInactive == DARK_COLORS) ? INVERT : DARK_COLORS;
    }
    if (event.key.code == sf::Keyboard::Escape) {
        selectedTroop = nullptr;
    }
    if (event.key.code == sf::Keyboard::F) {
        fullscreenMapMode = !fullscreenMapMode;
    }
    if (event.key.code == sf::Keyboard::G) {
        if (!changeTurnLocal && playersAmount > 1) {
            changeTurnLocal = true;
            nextTurn();
        } if (playersAmount == 1) {
            nextTurn();
        }
    }
    if (event.key.code == sf::Keyboard::Space) {
        changeTurnLocal = false;
        if (endGame) {
            exit(0);
        }
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
