#include "UIRenderer.h"

void UIRenderer::renderSidebar(sf::RenderWindow& window, const sf::Font& font, const std::string& infoText) {
    sf::Vector2u windowSize = window.getSize();

    float sidebarWidth = 250.f;
    sf::RectangleShape sidebar(sf::Vector2f(sidebarWidth, windowSize.y));
    sidebar.setPosition(windowSize.x - sidebarWidth, 0);
    sidebar.setFillColor(sf::Color(35, 35, 50, 230));

    sf::Text text(infoText, font, 16);
    text.setPosition(windowSize.x - sidebarWidth + 15, 20);
    text.setFillColor(sf::Color::White);

    window.draw(sidebar);
    window.draw(text);
}

void UIRenderer::renderBottomBar(sf::RenderWindow& window, const sf::Font& font, int turnCount) {
    sf::Vector2u windowSize = window.getSize();

    float barHeight = 60.f;
    sf::RectangleShape bottomBar(sf::Vector2f(windowSize.x, barHeight));
    bottomBar.setPosition(0, windowSize.y - barHeight);
    bottomBar.setFillColor(sf::Color(40, 40, 60, 230));

    sf::Text text("Ходов за игру: " + std::to_string(turnCount), font, 18);
    text.setPosition(20, windowSize.y - barHeight + 15);
    text.setFillColor(sf::Color::White);

    window.draw(bottomBar);
    window.draw(text);
}

