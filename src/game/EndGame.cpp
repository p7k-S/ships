#include "Game.h"
#include "../textures/EmbeddedResources.h"

void Game::renderVictoryScreen(uint8_t winnerId) {
    // Сохраняем текущее состояние
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());
    
    // Фон с затемнением
    window.clear(sf::Color(10, 10, 20, 230));
    
    sf::Vector2u windowSize = window.getSize();
    
    // Победный баннер
    sf::RectangleShape banner(sf::Vector2f(windowSize.x * 0.8f, 300));
    banner.setPosition(windowSize.x * 0.1f, windowSize.y * 0.15f);
    banner.setFillColor(sf::Color(40, 40, 60, 240));
    banner.setOutlineThickness(5);
    banner.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(banner);
    
    // Текст "ПОБЕДА!"
    sf::Text victoryText;
    victoryText.setFont(EmbeddedResources::main_font);
    victoryText.setString("VICTORY!");
    victoryText.setCharacterSize(72);
    victoryText.setFillColor(sf::Color(255, 215, 0));
    victoryText.setStyle(sf::Text::Bold);
    
    sf::FloatRect victoryBounds = victoryText.getLocalBounds();
    victoryText.setOrigin(victoryBounds.left + victoryBounds.width / 2.0f,
                         victoryBounds.top + victoryBounds.height / 2.0f);
    victoryText.setPosition(windowSize.x / 2.0f, banner.getPosition().y + 80);
    
    // Тень
    sf::Text victoryShadow = victoryText;
    victoryShadow.setFillColor(sf::Color(0, 0, 0, 150));
    victoryShadow.setPosition(victoryText.getPosition().x + 4, victoryText.getPosition().y + 4);
    
    window.draw(victoryShadow);
    window.draw(victoryText);
    
    // Имя победителя
    sf::Text winnerText;
    winnerText.setFont(EmbeddedResources::main_font);
    winnerText.setString("Winner: " + players[winnerId]->getName());
    winnerText.setCharacterSize(36);
    winnerText.setFillColor(sf::Color(255, 255, 255));
    
    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    winnerText.setOrigin(winnerBounds.left + winnerBounds.width / 2.0f,
                        winnerBounds.top + winnerBounds.height / 2.0f);
    winnerText.setPosition(windowSize.x / 2.0f, victoryText.getPosition().y + 100);
    
    window.draw(winnerText);
    
    // Таблица результатов
    sf::RectangleShape table(sf::Vector2f(windowSize.x * 0.7f, 250));
    table.setPosition(windowSize.x * 0.15f, banner.getPosition().y + banner.getSize().y + 40);
    table.setFillColor(sf::Color(30, 30, 45, 220));
    table.setOutlineThickness(3);
    table.setOutlineColor(sf::Color(60, 60, 80));
    window.draw(table);
    
    // Заголовок таблицы
    sf::Text tableTitle;
    tableTitle.setFont(EmbeddedResources::main_font);
    tableTitle.setString("FINAL RESULTS");
    tableTitle.setCharacterSize(28);
    tableTitle.setFillColor(sf::Color(200, 200, 255));
    tableTitle.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = tableTitle.getLocalBounds();
    tableTitle.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                        titleBounds.top + titleBounds.height / 2.0f);
    tableTitle.setPosition(windowSize.x / 2.0f, table.getPosition().y + 25);
    
    window.draw(tableTitle);
    
    // Отображаем всех игроков в таблице
    float startY = table.getPosition().y + 70;
    float rowHeight = 35;
    
    for (uint8_t i = 0; i < playersAmount; ++i) {
        // Фон строки (альтернирующие цвета)
        sf::RectangleShape rowBg(sf::Vector2f(table.getSize().x - 20, rowHeight - 5));
        rowBg.setPosition(table.getPosition().x + 10, startY + i * rowHeight);
        
        if (i == winnerId) {
            rowBg.setFillColor(sf::Color(50, 70, 40, 180)); // Зеленый для победителя
        } else if (players[i]->isDead()) {
            rowBg.setFillColor(sf::Color(70, 30, 30, 180)); // Красный для проигравших
        } else {
            rowBg.setFillColor(i % 2 == 0 ? sf::Color(40, 40, 55, 150) : sf::Color(45, 45, 60, 150));
        }
        
        window.draw(rowBg);
        
        // Данные игрока
        std::string playerStatus = players[i]->isDead() ? "ELIMINATED" : "ALIVE";
        std::string stats = players[i]->getName() + 
                          " | Troops: " + std::to_string(players[i]->getTroops().size()) +
                          " | Buildings: " + std::to_string(players[i]->getBuildings().size()) +
                          " | " + playerStatus;
        
        sf::Text playerText;
        playerText.setFont(EmbeddedResources::main_font);
        playerText.setString(stats);
        playerText.setCharacterSize(20);
        playerText.setFillColor(i == winnerId ? sf::Color(255, 255, 200) : 
                               players[i]->isDead() ? sf::Color(200, 150, 150) : 
                               sf::Color(200, 200, 220));
        
        playerText.setPosition(table.getPosition().x + 20, startY + i * rowHeight);
        window.draw(playerText);
    }
    
    sf::Text gameInfo;
    gameInfo.setFont(EmbeddedResources::main_font);
    gameInfo.setString("Total turns: " + std::to_string(totalTurnCount));
    gameInfo.setCharacterSize(24);
    gameInfo.setFillColor(sf::Color(180, 180, 220));

    // Вычисляем положение для центрирования
    sf::FloatRect gameInfoBounds = gameInfo.getLocalBounds();
    gameInfo.setOrigin(gameInfoBounds.left + gameInfoBounds.width / 2.0f,
            gameInfoBounds.top + gameInfoBounds.height / 2.0f);
    gameInfo.setPosition(windowSize.x / 2.0f, 
            table.getPosition().y + table.getSize().y + 30 + gameInfoBounds.height / 2.0f);

    window.draw(gameInfo);
 
    drawVictoryDecorations(window, winnerId);
    
    // Мерцающая подсказка внизу
    static sf::Clock hintClock;
    float hintAlpha = 128 + 127 * sin(hintClock.getElapsedTime().asSeconds() * 2);
    
    sf::Text hintText;
    hintText.setFont(EmbeddedResources::main_font);
    hintText.setString("ALT + F4...");
    hintText.setCharacterSize(20);
    hintText.setFillColor(sf::Color(200, 200, 220, static_cast<sf::Uint8>(hintAlpha)));
    
    sf::FloatRect hintBounds = hintText.getLocalBounds();
    hintText.setOrigin(hintBounds.left + hintBounds.width / 2.0f,
                      hintBounds.top + hintBounds.height / 2.0f);
    hintText.setPosition(windowSize.x / 2.0f, windowSize.y - 50);
    
    window.draw(hintText);
    
    window.display();
}

void Game::drawVictoryDecorations(sf::RenderWindow& window, uint8_t winnerId) {
    sf::Vector2u size = window.getSize();
    
    // Победные лучи от баннера
    static sf::Clock animClock;
    float time = animClock.getElapsedTime().asSeconds();
    
    sf::ConvexShape ray;
    ray.setPointCount(4);
    ray.setFillColor(sf::Color(255, 215, 0, 100));
    
    for (int i = 0; i < 8; ++i) {
        float angle = i * 45.0f + time * 20.0f;
        float length = 100 + 50 * sin(time * 2 + i);
        
        ray.setPoint(0, sf::Vector2f(0, -5));
        ray.setPoint(1, sf::Vector2f(length, -2));
        ray.setPoint(2, sf::Vector2f(length, 2));
        ray.setPoint(3, sf::Vector2f(0, 5));
        
        ray.setPosition(size.x / 2.0f, 150);
        ray.setRotation(angle);
        
        window.draw(ray);
    }
    
    // Корона над именем победителя
    sf::ConvexShape crown;
    crown.setPointCount(7);
    crown.setPoint(0, sf::Vector2f(0, 20));
    crown.setPoint(1, sf::Vector2f(10, 0));
    crown.setPoint(2, sf::Vector2f(20, 20));
    crown.setPoint(3, sf::Vector2f(30, 0));
    crown.setPoint(4, sf::Vector2f(40, 20));
    crown.setPoint(5, sf::Vector2f(20, 30));
    crown.setPoint(6, sf::Vector2f(0, 20));
    
    crown.setFillColor(sf::Color(255, 215, 0));
    crown.setOutlineThickness(2);
    crown.setOutlineColor(sf::Color(255, 255, 200));
    crown.setPosition(size.x / 2.0f - 20, 180);
    
    window.draw(crown);
}

void Game::showVictoryScreen(uint8_t winnerId) {
    bool victoryScreenActive = true;
    sf::Clock frameClock;
    
    while (window.isOpen() && victoryScreenActive) {
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            else if (event.type == sf::Event::KeyPressed) {
                // Любая клавиша закрывает экран победы
                victoryScreenActive = false;
                return;
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                // Клик мышью также закрывает экран победы
                victoryScreenActive = false;
                return;
            }
        }
        
        // Рендерим экран победы
        renderVictoryScreen(winnerId);
        
        // Ограничиваем FPS для плавной анимации
        sf::Time frameTime = frameClock.restart();
        if (frameTime < sf::milliseconds(16)) {
            sf::sleep(sf::milliseconds(16) - frameTime);
        }
    }
}

void Game::renderLoseScreen() {
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());
    
    // Фон с затемнением
    window.clear(sf::Color(30, 10, 10, 230));
    
    sf::Vector2u windowSize = window.getSize();
    
    // Баннер поражения
    sf::RectangleShape banner(sf::Vector2f(windowSize.x * 0.8f, 250));
    banner.setPosition(windowSize.x * 0.1f, windowSize.y * 0.2f);
    banner.setFillColor(sf::Color(60, 30, 30, 240));
    banner.setOutlineThickness(5);
    banner.setOutlineColor(sf::Color(180, 50, 50));
    window.draw(banner);
    
    // Текст "ПОРАЖЕНИЕ"
    sf::Text loseText;
    loseText.setFont(EmbeddedResources::main_font);
    loseText.setString("DEFEAT");
    loseText.setCharacterSize(72);
    loseText.setFillColor(sf::Color(220, 80, 80));
    loseText.setStyle(sf::Text::Bold);
    
    sf::FloatRect loseBounds = loseText.getLocalBounds();
    loseText.setOrigin(loseBounds.left + loseBounds.width / 2.0f,
                      loseBounds.top + loseBounds.height / 2.0f);
    loseText.setPosition(windowSize.x / 2.0f, banner.getPosition().y + 80);
    
    // Тень
    sf::Text loseShadow = loseText;
    loseShadow.setFillColor(sf::Color(0, 0, 0, 150));
    loseShadow.setPosition(loseText.getPosition().x + 4, loseText.getPosition().y + 4);
    
    window.draw(loseShadow);
    window.draw(loseText);
    
    // Сообщение
    sf::Text messageText;
    messageText.setFont(EmbeddedResources::main_font);
    messageText.setString("You were defeated by the bots");
    messageText.setCharacterSize(32);
    messageText.setFillColor(sf::Color(220, 180, 180));
    
    sf::FloatRect messageBounds = messageText.getLocalBounds();
    messageText.setOrigin(messageBounds.left + messageBounds.width / 2.0f,
                         messageBounds.top + messageBounds.height / 2.0f);
    messageText.setPosition(windowSize.x / 2.0f, loseText.getPosition().y + 90);
    
    window.draw(messageText);
    
    // Статистика игры
    sf::Text statsText;
    statsText.setFont(EmbeddedResources::main_font);
    statsText.setString("Turns survived: " + std::to_string(totalTurnCount));
    statsText.setCharacterSize(28);
    statsText.setFillColor(sf::Color(200, 150, 150));
    
    sf::FloatRect statsBounds = statsText.getLocalBounds();
    statsText.setOrigin(statsBounds.left + statsBounds.width / 2.0f,
                       statsBounds.top + statsBounds.height / 2.0f);
    statsText.setPosition(windowSize.x / 2.0f, messageText.getPosition().y + 60);
    
    window.draw(statsText);
    
    // Мерцающая подсказка внизу
    static sf::Clock hintClock;
    float hintAlpha = 128 + 127 * sin(hintClock.getElapsedTime().asSeconds() * 2);
    
    sf::Text hintText;
    hintText.setFont(EmbeddedResources::main_font);
    hintText.setString("Press any key to continue...");
    hintText.setCharacterSize(20);
    hintText.setFillColor(sf::Color(200, 150, 150, static_cast<sf::Uint8>(hintAlpha)));
    
    sf::FloatRect hintBounds = hintText.getLocalBounds();
    hintText.setOrigin(hintBounds.left + hintBounds.width / 2.0f,
                      hintBounds.top + hintBounds.height / 2.0f);
    hintText.setPosition(windowSize.x / 2.0f, windowSize.y - 50);
    
    window.draw(hintText);
    
    window.display();
}

void Game::showLoseScreen() {
    bool loseScreenActive = true;
    sf::Clock frameClock;
    
    while (window.isOpen() && loseScreenActive) {
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            else if (event.type == sf::Event::KeyPressed) {
                // Любая клавиша закрывает экран поражения
                loseScreenActive = false;
                return;
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                // Клик мышью также закрывает экран поражения
                loseScreenActive = false;
                return;
            }
        }
        
        // Рендерим экран поражения
        renderLoseScreen();
        
        // Ограничиваем FPS для плавной анимации
        sf::Time frameTime = frameClock.restart();
        if (frameTime < sf::milliseconds(16)) {
            sf::sleep(sf::milliseconds(16) - frameTime);
        }
    }
}
