#include "BaseBuild.h"

namespace GameLogic {
    void Building::lostResources(Building* enemy) {
    }
    uint8_t Building::getView() const {
        return 0; // Базовая видимость для зданий
    }
    
    uint16_t Building::getDamageRange() const {
        return 0; // Здания обычно не атакуют
    }
    
    // void Building::update() {
    //     // Базовая реализация  
    // }
    //
    // void Building::onDestroy() {
    //     // Базовая реализация
    // }
    //
    // uint16_t Building::getDefense() const {
    //     return 0;
    // }
}
