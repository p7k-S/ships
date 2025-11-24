#include <SFML/Graphics.hpp>
#include "game_network.h"

class TurnBasedGame {
private:
    sf::RenderWindow window;
    GameNetwork network;
    bool is_host;
    int current_player;
    std::vector<std::string> players;

public:
    TurnBasedGame() : window(sf::VideoMode(800, 600), "Turn-Based P2P Game"), current_player(0) {}
    
    void run() {
        // Выбор: хост или подключение
        if (showNetworkDialog()) {
            mainGameLoop();
        }
    }

private:
    bool showNetworkDialog() {
        // Простой UI для выбора режима
        sf::Font font;
        if (!font.loadFromFile("/home/zane/.fonts/oldschool/ttf - Px (pixel outline)/Px437_IBM_BIOS.ttf")) return false;
        
        sf::Text host_text("Press H to Host Game", font, 24);
        sf::Text join_text("Press J to Join Game", font, 24);
        host_text.setPosition(300, 250);
        join_text.setPosition(300, 300);
        
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return false;
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::H) {
                        // Создаем хост
                        is_host = true;
                        if (network.startHost(53000)) {
                            players.push_back("Host Player");
                            return true;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::J) {
                        // Подключаемся к хосту
                        is_host = false;
                        if (network.connectToPeer("localhost", 53000)) {
                            players.push_back("Client Player");
                            return true;
                        }
                    }
                }
            }
            
            window.clear();
            window.draw(host_text);
            window.draw(join_text);
            window.display();
        }
        return false;
    }

    void mainGameLoop() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Обработка хода игрока
            if (event.type == sf::Event::MouseButtonPressed && isMyTurn()) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    makeMove(event.mouseButton.x, event.mouseButton.y);
                }
            }
        }
        
        // Обработка сетевых сообщений
        while (network.hasMessages()) {
            processNetworkMessage(network.popMessage());
        }
    }

    void update() {
        // Обновление состояния игры
    }

    void render() {
        window.clear(sf::Color::White);
        
        // Отрисовка игрового поля
        sf::RectangleShape cell(sf::Vector2f(50, 50));
        cell.setOutlineColor(sf::Color::Black);
        cell.setOutlineThickness(1);
        
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cell.setPosition(300 + i * 60, 200 + j * 60);
                window.draw(cell);
            }
        }
        
        // Отрисовка чей ход
        sf::Font font;
        if (font.loadFromFile("/home/zane/.fonts/oldschool/ttf - Mx (mixed outline+bitmap)/MxPlus_IBM_CGA.ttf")) {
            sf::Text turn_text("Current turn: Player " + std::to_string(current_player + 1), font, 20);
            turn_text.setPosition(300, 150);
            turn_text.setFillColor(sf::Color::Black);
            window.draw(turn_text);
        }
        
        window.display();
    }

    bool isMyTurn() {
        // Для упрощения: ход определяется по индексу игрока
        return current_player == 0; // Первый игрок всегда локальный
    }

    void makeMove(int x, int y) {
        // Преобразование координат в клетку
        int cell_x = (x - 300) / 60;
        int cell_y = (y - 200) / 60;
        
        if (cell_x >= 0 && cell_x < 3 && cell_y >= 0 && cell_y < 3) {
            std::string move = "MOVE|" + std::to_string(cell_x) + "|" + std::to_string(cell_y);
            network.sendToAll(move);
            
            // Передаем ход следующему игроку
            current_player = (current_player + 1) % network.getPeerCount();
        }
    }

    void processNetworkMessage(const std::string& message) {
        // Простой парсинг сообщений
        if (message.find("MOVE|") == 0) {
            // Обработка хода другого игрока
            std::cout << "Opponent made move: " << message << std::endl;
            current_player = (current_player + 1) % network.getPeerCount();
        }
        else if (message.find("WELCOME|") == 0) {
            // Новый игрок подключился
            std::string player_name = message.substr(8);
            players.push_back(player_name);
            std::cout << "New player: " << player_name << std::endl;
        }
    }
};

int main() {
    TurnBasedGame game;
    game.run();
    return 0;
}
