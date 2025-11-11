#include "../GameLogic.h"
#include "Player.h"

namespace GameLogic
{
    bool isEnemy(std::variant<Player*, Enemy*, Pirate*> p1, 
                 std::variant<Player*, Enemy*, Pirate*> p2) {
        if (!std::holds_alternative<Player*>(p1) ||
            !std::holds_alternative<Player*>(p2)) {
            if (p1.index() == p2.index()) {
                return false;           // если оба не player и оба одного класса
            }
            return true;                // если player и не player
        }

        Player* player1 = std::get<Player*>(p1);
        Player* player2 = std::get<Player*>(p2);

        std::vector<uint8_t> friendsIds = player2->getFriendsIds();
        uint8_t secondP_id = player1->getId();

        for (auto& id : friendsIds) {
            if (secondP_id == id)
                return false;
        }

        return true;
    }
} // namespace GameLogic
