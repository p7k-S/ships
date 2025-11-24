#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <string>

class GameClient {
public:
    GameClient();
    ~GameClient();
    
    bool connectToServer(const std::string& serverIp, unsigned short serverPort);
    void disconnect();
    void update();
    void sendToServer(const std::string& message);
    bool hasNewMessages();
    std::string getNextMessage();
    bool isConnected() const;

private:
    void handleServerMessages();
    
    std::unique_ptr<sf::TcpSocket> serverSocket;
    std::vector<std::string> messageQueue;
    bool connected;
};
