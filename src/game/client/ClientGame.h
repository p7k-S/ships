#include "Game.h"

class ClientGame {
public:
    void run();
    
private:
    std::unique_ptr<GameClient> gameClient;
    sf::RenderWindow window;
    
    // Только для отображения
    std::vector<gl::Hex> hexMap;
    std::vector<std::unique_ptr<gl::Player>> players;
    
    uint8_t myPlayerId = 0;
    uint8_t currentPlayerId = 0;
    
    void handleNetworkMessages();
    void processServerMessage(const std::string& msg);
    void sendPlayerAction(int fromQ, int fromR, int toQ, int toR);
    void render();
};
