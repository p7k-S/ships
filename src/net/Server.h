#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <string>

class GameServer {
public:
    GameServer();
    ~GameServer();
    
    bool start(unsigned short port);
    void stop();
    void update();
    void broadcastToAll(const std::string& message);
    bool hasNewMessages();
    std::string getNextMessage();
    unsigned short getPort() const;

private:
    void handleNewConnections();
    void handleClientMessages();
    
    unsigned short actualPort = 0;
    sf::TcpListener listener;
    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    std::vector<std::string> messageQueue;
    bool running;
};
