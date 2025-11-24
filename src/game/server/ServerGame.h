#pragma once
#include "../../net/Server.h"
#include "../GameLogic.h"

namespace gl = GameLogic;

class ServerGame {
public:
    void run();
    
private:
    std::unique_ptr<GameServer> gameServer;
    std::vector<std::unique_ptr<gl::Player>> players;
    std::vector<GameLogic::Hex> hexMap;
    GameLogic::Enemy enemy{"Enemy"};
    GameLogic::Pirate pirate{"Pirate"};
    
    uint8_t connectedPlayers = 0;
    uint8_t playersAmount = 0;
    uint8_t currentPlayerId = 0;
    
    void handleNetworkMessages();
    void processClientMessage(const std::string& msg);
    void executeNetworkAction(const std::string& msg);
    void broadcastGameState();
    void updateGameLogic();
};
