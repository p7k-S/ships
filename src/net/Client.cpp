#include "Client.h"
#include <iostream>

GameClient::~GameClient() { disconnect(); }

GameClient::GameClient() : connected(false) {}

bool GameClient::connectToServer(const std::string& serverIp, unsigned short serverPort) {
    serverSocket = std::make_unique<sf::TcpSocket>();
    
    if (serverSocket->connect(serverIp, serverPort) == sf::Socket::Done) {
        connected = true;
        std::cout << "Connected to server " << serverIp << ":" << serverPort << std::endl;
        return true;
    } else {
        std::cout << "Failed to connect to server " << serverIp << ":" << serverPort << std::endl;
        connected = false;
        return false;
    }
}

void GameClient::disconnect() {
    connected = false;
    if (serverSocket) {
        serverSocket->disconnect();
    }
}

void GameClient::update() {
    if (!connected) return;
    handleServerMessages();
}

void GameClient::handleServerMessages() {
    if (!serverSocket) return;
    
    sf::Packet packet;
    sf::Socket::Status status = serverSocket->receive(packet);
    
    if (status == sf::Socket::Done) {
        std::string message;
        if (packet >> message) {
            messageQueue.push_back(message);
            std::cout << "Received from server: " << message << std::endl;
        }
    } else if (status == sf::Socket::Disconnected) {
        std::cout << "Disconnected from server" << std::endl;
        connected = false;
    }
}

void GameClient::sendToServer(const std::string& message) {
    if (!connected || !serverSocket) return;
    
    sf::Packet packet;
    packet << message;
    
    if (serverSocket->send(packet) == sf::Socket::Done) {
        std::cout << "Sent to server: " << message << std::endl;
    } else {
        std::cout << "Failed to send message to server" << std::endl;
    }
}

bool GameClient::hasNewMessages() {
    return !messageQueue.empty();
}

std::string GameClient::getNextMessage() {
    if (messageQueue.empty()) return "";
    std::string message = messageQueue.front();
    messageQueue.erase(messageQueue.begin());
    return message;
}

bool GameClient::isConnected() const {
    return connected;
}
