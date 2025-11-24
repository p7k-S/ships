#include "Game.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include "../net/NetworkConfig.h"
#include "GameConfig.h"
#include <cstdlib>
// #include <sstream>
#include <string>


Game::Game() {}


void Game::sendPlayers() {
    std::string msg = "PLAYERS:" + std::to_string(players.size()) + ":";
    
    for (auto& player : players) {
        // Сериализуем только данные, которые можно безопасно передать
        msg += std::to_string(player->getId()) + "," +
               player->getName() + "," +
               std::to_string(player->getColor().toInteger()) + "," +
               std::to_string(player->isDead()) + "|";
    }
    
    sendNetworkMessage(msg);
    std::cout << "Sent players data: " << players.size() << " players" << std::endl;
}

void Game::receivePlayers(const std::string& msg) {
    size_t pos = 8; // после "PLAYERS:"
    size_t colon = msg.find(":", pos);
    
    int playerCount = std::stoi(msg.substr(pos, colon - pos));
    players.clear();
    
    std::string playersData = msg.substr(colon + 1);
    size_t playerStart = 0;
    size_t playerEnd = 0;
    
    for (int i = 0; i < playerCount; ++i) {
        playerEnd = playersData.find("|", playerStart);
        if (playerEnd == std::string::npos) break;
        
        std::string playerStr = playersData.substr(playerStart, playerEnd - playerStart);
        
        // Парсим: id,name,color,gold,isDead
        std::vector<std::string> parts;
        size_t partStart = 0;
        size_t partEnd = 0;
       
        while ((partEnd = playerStr.find(",", partStart)) != std::string::npos) {
            parts.push_back(playerStr.substr(partStart, partEnd - partStart));
            partStart = partEnd + 1;
        }
        parts.push_back(playerStr.substr(partStart));
        
        if (parts.size() >= 5) {
            uint8_t id = std::stoi(parts[0]);
            std::string name = parts[1];
            sf::Uint32 colorValue = std::stoul(parts[2]);
            int gold = std::stoi(parts[3]);
            bool isDead = std::stoi(parts[4]);
            
            // Создаем игрока
            sf::Color color(colorValue);
            auto player = std::make_unique<gl::Player>(name, color);
            
            // Восстанавливаем основные параметры
            // player->setGold(gold); // если есть сеттер
            // player->setDead(isDead); // если есть сеттер
            
            players.push_back(std::move(player));
            
            std::cout << "Received player: " << name << " (ID: " << (int)id << ")" << std::endl;
        }
        
        playerStart = playerEnd + 1;
    }
    
    // Устанавливаем my_pid для клиента
    if (!isHost && !players.empty()) {
        // Находим себя по имени или другому критерию
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i]->getName() == players[0]->getName()) { // временная логика
                my_pid = i;
                break;
            }
        }
        std::cout << "I am player " << (int)my_pid << std::endl;
    }
}

void Game::sendTroopAction(int fromQ, int fromR, int toQ, int toR) {
    std::string msg = "ACTION:" + 
                     std::to_string(my_pid) + ":" +
                     std::to_string(fromQ) + ":" + std::to_string(fromR) + ":" +
                     std::to_string(toQ) + ":" + std::to_string(toR);
    
    if (isHost && gameServer) {
        gameServer->broadcastToAll(msg);
    } else if (!isHost && gameClient) {
        gameClient->sendToServer(msg);
    }
    
    std::cout << "Sent action: " << msg << std::endl;
}

void Game::executeNetworkAction(const std::string& msg) {
    // Парсим: "ACTION:playerId:fromQ:fromR:toQ:toR"
    size_t pos1 = 7; // после "ACTION:"
    size_t pos2 = msg.find(":", pos1);
    size_t pos3 = msg.find(":", pos2 + 1);
    size_t pos4 = msg.find(":", pos3 + 1);
    size_t pos5 = msg.find(":", pos4 + 1);
    
    int playerId = std::stoi(msg.substr(pos1, pos2 - pos1));
    int fromQ = std::stoi(msg.substr(pos2 + 1, pos3 - pos2 - 1));
    int fromR = std::stoi(msg.substr(pos3 + 1, pos4 - pos3 - 1));
    int toQ = std::stoi(msg.substr(pos4 + 1, pos5 - pos4 - 1));
    int toR = std::stoi(msg.substr(pos5 + 1));
    
    int fromIndex = fromQ + fromR * mapWidth;
    int toIndex = toQ + toR * mapWidth;

    if (hexMap[fromIndex].hasTroop()) {
        selectedTroop = hexMap[fromIndex].getTroop();
        targetHex = &hexMap[toIndex];
        executeTroopAction();
    }
}


void Game::handleNetworkMessages() {
    if (isHost && gameServer) {
        // Хост обрабатывает сообщения от клиентов
        gameServer->update();
        while (gameServer->hasNewMessages()) {
            std::string msg = gameServer->getNextMessage();
            std::cout << "SERVER: " << msg << std::endl;
            processServerMessage(msg);
        }
    } else if (!isHost && gameClient) {
        // Клиент обрабатывает сообщения от сервера
        gameClient->update();
        while (gameClient->hasNewMessages()) {
            std::string msg = gameClient->getNextMessage();
            std::cout << "CLIENT: " << msg << std::endl;
            processClientMessage(msg);
        }
    }
}

void Game::processServerMessage(const std::string& msg) {
    if (msg.find("JOIN_REQUEST:") == 0) {
        // Хост получает запрос на подключение от клиента
        if (connectedPlayers < playersAmount) {
            size_t pos1 = msg.find(":") + 1;
            size_t pos2 = msg.find(":", pos1);
            std::string playerName = msg.substr(pos1, pos2 - pos1);
            std::string playerColor = msg.substr(pos2 + 1);
            
            // Хост создает игрока для клиента
            auto player = std::make_unique<gl::Player>(playerName, COLORS[playerColor]);
            players.push_back(std::move(player));
            
            connectedPlayers++;
            std::cout << "Player " << playerName << " connected! (" << connectedPlayers << "/" << playersAmount << ")" << std::endl;
            
            // Хост подтверждает подключение всем клиентам
            gameServer->broadcastToAll("PLAYER_ACCEPTED:" + std::to_string(connectedPlayers-1));
        }
    }
    else if (msg.find("ACTION:") == 0) {
        // ВЫЗОВ: клиент прислал действие - выполняем и рассылаем
        executeNetworkAction(msg);
        gameServer->broadcastToAll(msg); // рассылаем всем клиентам
    }

    // Добавьте другие обработки сообщений для сервера
}

void Game::processClientMessage(const std::string& msg) {
    if (msg.find("PLAYER_ACCEPTED:") == 0) {
        // Клиент получает подтверждение от хоста
        int playerIndex = std::stoi(msg.substr(16));
        std::cout << "Host accepted me as player " << playerIndex << "!" << std::endl;
    }
    else if (msg.find("ACTION:") == 0) {
        // Парсим playerId
        size_t pos1 = 7;
        size_t pos2 = msg.find(":", pos1);
        int playerId = std::stoi(msg.substr(pos1, pos2 - pos1));

        // Выполняем только если действие не от нас
        if (playerId != my_pid) {
            executeNetworkAction(msg);
        }
    }


    // Добавьте другие обработки сообщений для клиента
}

void Game::createLocalPlayer() {
    std::string name, color;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter your color: ";
    std::cin >> color;

    auto player = std::make_unique<gl::Player>(name, COLORS[color]);
    players.push_back(std::move(player));
    
    connectedPlayers = 1;
    
    if (!isHost && gameClient) {
        // Клиент отправляет запрос на подключение серверу
        gameClient->sendToServer("JOIN_REQUEST:" + name + ":" + color);
    }
}

void Game::run() {
    // Спросить тип игры
    std::cout << "Local game (1) or Network game (2)? ";
    int choice;
    std::cin >> choice;
    isNetworkGame = (choice == 2);
    
    if (isNetworkGame) {
        std::cout << "Host (1) or Client (2)? ";
        std::cin >> choice;
        isHost = (choice == 1);
        
        // Инициализация сети в зависимости от роли
        if (isHost) {
            gameServer = std::make_unique<GameServer>();
            if (!gameServer->start(GAME_PORT)) {
                std::cout << "Failed to start server!" << std::endl;
                return;
            }
            
            if (!startMenu()) return;
            createLocalPlayer();
            std::cout << "Waiting for players... (" << connectedPlayers << "/" << playersAmount << ")" << std::endl;
            std::cout << "Tell clients to connect to port: " << gameServer->getPort() << std::endl;
            
        } else {
            gameClient = std::make_unique<GameClient>();
            std::string hostIp;
            unsigned short hostPort;
            std::cout << "Enter host IP: ";
            std::cin >> hostIp;
            std::cout << "Enter host PORT: ";
            std::cin >> hostPort;
            
            if (!gameClient->connectToServer(hostIp, hostPort)) {
                std::cout << "Failed to connect to server!" << std::endl;
                return;
            }
            
            createLocalPlayer();
            std::cout << "Waiting for host to accept..." << std::endl;
        }
    } else {
        // Одиночная игра
        if (!startMenu()) return;
    }

    if (!initialize()) return;

    // Для одиночной игры
    if (!isNetworkGame) {
        if (players.empty()) {
            createPlayers();
        }
        createTroops();
    }

    // Если сетевая игра и хост - ждем подключения всех игроков
    if (isNetworkGame && isHost) {
        while (connectedPlayers < playersAmount) {
            handleNetworkMessages();
            sf::sleep(sf::milliseconds(100));
        }
        
        // Уведомляем всех, что игра начинается
        gameServer->broadcastToAll("GAME_STARTING");
        createTroops(); // пока только корабли нужно сделать порты
    }


    // Если клиент - ждем начала игры
    // if (isNetworkGame && !isHost) {
    //     bool gameStarting = false;
    //     while (!gameStarting) {
    //         handleNetworkMessages();
    //         // Проверяем, не пришло ли сообщение о начале игры
    //         // (нужно добавить эту обработку в processClientMessage)
    //         sf::sleep(sf::milliseconds(100));
    //     }
    // }

    std::cout << "Game starting with " << players.size() << " players" << std::endl;

    while (window.isOpen()) {
        processEvents();

        // if (isNetworkGame) {
        //     handleNetworkMessages();
        // }


        if (isProcessingTurn) {
            isProcessingTurn = false;
            p_id = nextAlivePlayer();

            // if (isNetworkGame) {
            //     sendTurnChange(p_id);
            // }
        }

        if (changeTurnLocal) {
            renderWaitMove();
        }
        else {
            render();
        }
    }
}

uint8_t Game::nextAlivePlayer() {
    totalTurnCount++;
        std::cout << "=== DEBUG: Checking alive players ===" << std::endl;
    for (uint8_t i = 0; i < playersAmount; ++i) {
        std::cout << "Player " << (int)i << " - troops: " << players[i]->getTroops().size() 
                  << ", buildings: " << players[i]->getBuildings().size()
                  << ", isDead: " << players[i]->isDead() << std::endl;
    }

    if (playersAmount > 1) {
        for (uint8_t i = 1; i < playersAmount; ++i) {
            uint8_t nextPlayer = (p_id + i) % playersAmount;
            std::cout << "Checking player " << (int)nextPlayer << " - isDead: " << players[nextPlayer]->isDead() << std::endl;

            if (!players[(p_id + i) % playersAmount]->isDead()) {
                return (p_id + i) % playersAmount;
            }
        }
        std::cout << "The winner is " << players[p_id]->getName() << "!!!" << std::endl;
        std::cout << "Game turns " << totalTurnCount << std::endl;
        std::exit(0);
    } else {
        return 0;
    }
}

// Добавьте метод для отправки сетевых сообщений
void Game::sendNetworkMessage(const std::string& message) {
    if (isNetworkGame) {
        if (isHost && gameServer) {
            gameServer->broadcastToAll(message);
        } else if (!isHost && gameClient) {
            gameClient->sendToServer(message);
        }
    }
}

// void Game::processPlayerTurn() {
//     updateVisibleCells();
// }

bool Game::isPlayerOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Player*>(owner);
}

bool Game::isEnemyOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Enemy*>(owner);
}

bool Game::isPirateOwner(const GameLogic::Owner& owner) const {
    return std::holds_alternative<gl::Pirate*>(owner);
}

// void Game::sendMap()
// {
//     std::ostringstream out;
//     out << "MAP_BEGIN\n";
//
//     for (const auto& h : hexMap)
//     {
//         out << h.q << " "
//             << h.r << " "
//             << static_cast<int>(h.getCellType()) << " "
//             << h.getNoise() << " "
//             << h.getGold() << "\n";
//     }
//
//     out << "MAP_END";
//
//     std::string msg = out.str();
//     sendNetworkMessage(msg);
// }
//
// void Game::receiveMap(const std::string& msg)
// {
//     std::istringstream in(msg);
//     std::string line;
//
//     // Ждём MAP_BEGIN
//     std::getline(in, line);
//     if (line != "MAP_BEGIN") return;
//
//     hexMap.clear();
//
//     while (std::getline(in, line))
//     {
//         if (line == "MAP_END") break;
//
//         std::istringstream row(line);
//
//         int q, r;
//         int type;
//         float noise;
//         uint16_t gold;
//
//         row >> q >> r >> type >> noise >> gold;
//
//         GameLogic::Hex hex(q, r);
//         hex.setCellType(static_cast<gl::CellType>(type));
//
//         // шум
//         // если хочешь — сделай метод setNoise()
//         *(float*)&hex = noise; // временно напрямую
//
//         // золото
//         if (gold > 0)
//             hex.addGold(gold);
//
//         hexMap.push_back(hex);
//     }
// }
