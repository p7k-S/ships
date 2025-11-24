#include "Server.h"
#include <iostream>

GameServer::~GameServer() { stop(); }

GameServer::GameServer() : running(false), actualPort(0) {}

bool GameServer::start(unsigned short port) {
    for (unsigned short tryPort = port; tryPort < port + 10; ++tryPort) {
        if (listener.listen(tryPort) == sf::Socket::Done) {
            running = true;
            actualPort = tryPort;
            std::cout << "Game server started on port " << actualPort << std::endl;
            return true;
        }
    }
    
    std::cout << "Failed to start server on any port!" << std::endl;
    return false;
}

unsigned short GameServer::getPort() const {
    return actualPort;
}

void GameServer::stop() {
    running = false;
    listener.close();
    clients.clear();
}

void GameServer::update() {
    if (!running) return;
    handleNewConnections();
    handleClientMessages();
}

void GameServer::handleNewConnections() {
    auto clientSocket = std::make_unique<sf::TcpSocket>();
    if (listener.accept(*clientSocket) == sf::Socket::Done) {
        // Отправляем клиенту информацию о сервере
        sf::Packet welcomePacket;
        welcomePacket << "SERVER_WELCOME:" << actualPort;
        clientSocket->send(welcomePacket);
        
        clients.push_back(std::move(clientSocket));
        std::cout << "New client connected! Total clients: " << clients.size() << std::endl;
    }
}

void GameServer::handleClientMessages() {
    for (auto& client : clients) {
        sf::Packet packet;
        sf::Socket::Status status = client->receive(packet);
        
        if (status == sf::Socket::Done) {
            std::string message;
            if (packet >> message) {
                messageQueue.push_back(message);
                std::cout << "Received from client: " << message << std::endl;
            }
        } else if (status == sf::Socket::Disconnected) {
            std::cout << "Client disconnected" << std::endl;
            // нужно удалить клиента из вектора
        }
    }
}

void GameServer::broadcastToAll(const std::string& message) {
    sf::Packet packet;
    packet << message;
    
    for (auto& client : clients) {
        client->send(packet);
    }
    
    std::cout << "Broadcasted to " << clients.size() << " clients: " << message << std::endl;
}

bool GameServer::hasNewMessages() {
    return !messageQueue.empty();
}

std::string GameServer::getNextMessage() {
    if (messageQueue.empty()) return "";
    std::string message = messageQueue.front();
    messageQueue.erase(messageQueue.begin());
    return message;
}
