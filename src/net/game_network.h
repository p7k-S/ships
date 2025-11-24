#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

class GameNetwork {
private:
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> peers;
    sf::SocketSelector selector;
    std::atomic<bool> running;
    std::thread network_thread;
    
    // Очередь входящих сообщений
    std::queue<std::string> message_queue;
    std::mutex queue_mutex;

public:
    GameNetwork();
    ~GameNetwork();
    
    bool startHost(unsigned short port);
    bool connectToPeer(const std::string& ip, unsigned short port);
    void sendToAll(const std::string& message);
    void sendToPeer(int peer_index, const std::string& message);
    bool hasMessages();
    std::string popMessage();
    void stop();
    int getPeerCount() const { return peers.size(); }

private:
    void networkLoop();
    void handleNewConnection();
    void handlePeerData(sf::TcpSocket* peer);
};
