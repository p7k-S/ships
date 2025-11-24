#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>

class SimpleP2PNode {
private:
    zmq::context_t context;
    zmq::socket_t publisher;  // Для отправки сообщений
    zmq::socket_t subscriber; // Для получения сообщений
    std::string node_id;
    int pub_port;
    int sub_port;

public:
    SimpleP2PNode(const std::string& id, int pub_port, int sub_port) 
        : context(1),
          publisher(context, ZMQ_PUB),
          subscriber(context, ZMQ_SUB),
          node_id(id),
          pub_port(pub_port),
          sub_port(sub_port) {
        
        // Настраиваем сокет для публикации (отправки)
        publisher.bind("tcp://*:" + std::to_string(pub_port));
        
        // Настраиваем сокет для подписки (получения)
        subscriber.bind("tcp://*:" + std::to_string(sub_port));
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0); // Подписываемся на все сообщения
        
        std::cout << "Узел " << node_id << " запущен." << std::endl;
        std::cout << "Публикация на порту: " << pub_port << std::endl;
        std::cout << "Подписка на порту: " << sub_port << std::endl;
    }

    // Подключиться к другому узлу
    void connectToNode(const std::string& address, int port) {
        std::string connect_str = "tcp://" + address + ":" + std::to_string(port);
        subscriber.connect(connect_str);
        std::cout << "Подключились к узлу: " << connect_str << std::endl;
    }

    // Отправить сообщение
    void sendMessage(const std::string& message) {
        std::string full_msg = "[" + node_id + "] " + message;
        zmq::message_t zmq_msg(full_msg.size());
        memcpy(zmq_msg.data(), full_msg.c_str(), full_msg.size());
        publisher.send(zmq_msg);
        std::cout << "Отправлено: " << full_msg << std::endl;
    }

    // Получить сообщения
    void receiveMessages() {
        while (true) {
            zmq::message_t message;
            if (subscriber.recv(&message, ZMQ_NOBLOCK)) {
                std::string msg_str(static_cast<char*>(message.data()), message.size());
                std::cout << "Получено: " << msg_str << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Запустить в отдельном потоке
    void startReceiver() {
        std::thread receiver_thread(&SimpleP2PNode::receiveMessages, this);
        receiver_thread.detach();
    }
};
