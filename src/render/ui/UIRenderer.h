#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class UIRenderer {
public:
    static void renderSidebar(sf::RenderWindow& window, const sf::Font& font, const std::string& infoText);
    static void renderBottomBar(sf::RenderWindow& window, const sf::Font& font, int turnCount);
};

