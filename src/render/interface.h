void showInterface() {
    if (selectedHex) {
        if (selectedHex->hasShip()) {
            selectedShip = selectedHex->getShip();
        }
        sf::Text shipInfo;
        shipInfo.setFont(font);
        shipInfo.setString(
                "Cell (q;r): (" + std::to_string(selectedShip->getCell()->q) + 
                ";" +
                std::to_string(selectedShip->getCell()->r) + ")"
                "\n\nShip:\nHP: " + std::to_string(selectedShip->getHealth()) + 
                // "\nOwner: " + std::to_string(selectedShip->getOwner()) +
                "\nDamage: " + std::to_string(selectedShip->getDamage()) +
                "\nGold: " + std::to_string(selectedShip->getGold()) +
                "/" + std::to_string(selectedShip->getMaxGold()) +
                "\n\nView range: " + std::to_string(selectedShip->getView()) +
                "\nMove range: " + std::to_string(selectedShip->getMoveRange()) +
                "\nDamage range: " + std::to_string(selectedShip->getDamageRange())
                );
        shipInfo.setCharacterSize(16);
        shipInfo.setFillColor(sf::Color::White);
        shipInfo.setPosition(mapWidth + 20.f, 50.f); // убрал умножение на 10
        window.draw(shipInfo);
    }

    // Область под картой для дополнительной информации - высота 250 пикселей
    sf::RectangleShape bottomArea(sf::Vector2f(window.getSize().x, 250.f));
    bottomArea.setPosition(0, window.getSize().y - 250.f); // прижимаем к низу окна
    bottomArea.setFillColor(sf::Color(30, 30, 30, 200));
    window.draw(bottomArea);

    sf::Text bottomText;
    bottomText.setFont(font);
    bottomText.setString("Control: WASD - camera, +/- - zoom, Enter - apply move");
    bottomText.setCharacterSize(14);
    bottomText.setFillColor(sf::Color::White);
    bottomText.setPosition(10.f, window.getSize().y - 250.f + 10.f); // позиционируем внутри нижней области
    window.draw(bottomText);
}
