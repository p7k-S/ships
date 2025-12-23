#include "../GameLogic.h"
#include "Player.h"

namespace GameLogic
{
    bool isEnemy(std::variant<Player*, Enemy*, Pirate*> p1, 
            std::variant<Player*, Enemy*, Pirate*> p2) {
        if (p1 == p2) return false;

        // Разные типы
        if (p1.index() != p2.index()) return true;

        if (auto* player1 = std::get_if<Player*>(&p1)) {
            if (auto* player2 = std::get_if<Player*>(&p2)) {
                return !areFriends(*player1, *player2);
            }
        }

        return true;
    }

    bool areFriends(Player* p1, Player* p2) {
        auto friends1 = p1->getFriendsIds();
        auto friends2 = p2->getFriendsIds();

        bool p1FriendsWithP2 = std::find(friends1.begin(), friends1.end(), p2->getId()) != friends1.end();
        bool p2FriendsWithP1 = std::find(friends2.begin(), friends2.end(), p1->getId()) != friends2.end();

        return p1FriendsWithP2 && p2FriendsWithP1;
    }
} // namespace GameLogic
