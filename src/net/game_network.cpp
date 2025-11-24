#include "game_network.h"

GameNetwork::GameNetwork() : running(false) {
    selector.add(listener);
}

GameNetwork::~GameNetwork() {
    stop();
}

bool GameNetwork::startHost(unsigned short port) {
    if (listener.listen(port) != sf::Socket::Done) {
        std::cout << "Failed to start server on port " << port << std::endl;
        return false;
    }
    
    running = true;
    network_thread = std::thread(&GameNetwork::networkLoop, this);
    std::cout << "Host started on port " << port << std::endl;
    return true;
}

bool GameNetwork::connectToPeer(const std::string& ip, unsigned short port) {
    sf::TcpSocket* socket = new sf::TcpSocket;
    if (socket->connect(ip, port) != sf::Socket::Done) {
        delete socket;
        std::cout << "Failed to connect to " << ip << ":" << port << std::endl;
        return false;
    }
    
    peers.push_back(socket);
    selector.add(*socket);
    std::cout << "Connected to " << ip << ":" << port << std::endl;
    return true;
}

void GameNetwork::networkLoop() {
    while (running) {
        if (selector.wait(sf::milliseconds(100))) {
            // Новое подключение
            if (selector.isReady(listener)) {
                handleNewConnection();
            }
            
            // Данные от пиров
            for (auto& peer : peers) {
                if (selector.isReady(*peer)) {
                    handlePeerData(peer);
                }
            }
        }
    }
}

void GameNetwork::handleNewConnection() {
    sf::TcpSocket* socket = new sf::TcpSocket;
    if (listener.accept(*socket) == sf::Socket::Done) {
        peers.push_back(socket);
        selector.add(*socket);
        std::cout << "New peer connected!" << std::endl;
        
        // Отправляем приветственное сообщение
        sendToPeer(peers.size() - 1, "WELCOME|Player" + std::to_string(peers.size()));
    } else {
        delete socket;
    }
}

void GameNetwork::handlePeerData(sf::TcpSocket* peer) {
    char buffer[1024];
    std::size_t received;
    if (peer->receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        message_queue.push(std::string(buffer, received));
        std::cout << "Received: " << std::string(buffer, received) << std::endl;
    }
}

void GameNetwork::sendToAll(const std::string& message) {
    for (size_t i = 0; i < peers.size(); ++i) {
        sendToPeer(i, message);
    }
}

void GameNetwork::sendToPeer(int peer_index, const std::string& message) {
    if (peer_index >= 0 && peer_index < peers.size()) {
        if (peers[peer_index]->send(message.c_str(), message.size() + 1) != sf::Socket::Done) {
            std::cout << "Failed to send message to peer " << peer_index << std::endl;
        }
    }
}

bool GameNetwork::hasMessages() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    return !message_queue.empty();
}

std::string GameNetwork::popMessage() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    if (message_queue.empty()) return "";
    
    std::string message = message_queue.front();
    message_queue.pop();
    return message;
}

void GameNetwork::stop() {
    running = false;
    if (network_thread.joinable()) {
        network_thread.join();
    }
    
    for (auto peer : peers) {
        delete peer;
    }
    peers.clear();
}
