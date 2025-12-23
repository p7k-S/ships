#include "BaseBuild.h"

namespace GameLogic {
    uint8_t Building::getView() const {
        return 0; // Базовая видимость для зданий
    }
    
    uint16_t Building::getDamageRange() const {
        return 0; // Здания обычно не атакуют
    }
}
