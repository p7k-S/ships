#include "Game.h"

void Game::handleNetworkMessage(const std::string& msg) {
    std::cout << "Network: " << msg << std::endl;
    
    // Парсинг сообщений
    if (msg.find("PLAYER_MOVE:") == 0) {
        // Обработка движения другого игрока
        int x, y;
        sscanf(msg.c_str(), "PLAYER_MOVE:%d,%d", &x, &y);
        updateRemotePlayerPosition(x, y);
    }
    else if (msg.find("TURN_END:") == 0) {
        // Другой игрок закончил ход
        isProcessingTurn = false;
    }
    else if (msg.find("CHAT:") == 0) {
        // Чат сообщение
        std::string chatMsg = msg.substr(5);
        addChatMessage(chatMsg);
    }
}
