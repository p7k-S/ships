cmake_minimum_required(VERSION 3.10)

project(GameProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2 REQUIRED graphics window)

add_custom_target(embedded_resources ALL 
    DEPENDS 
        src/textures/embedded_resources.h
        src/textures/UIfont.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/game
    ${CMAKE_SOURCE_DIR}/src/render
    ${CMAKE_SOURCE_DIR}/src/render/map
    ${CMAKE_SOURCE_DIR}/src/textures
)

set(SOURCE_FILES
    src/main.cpp
    src/game/map/GenerateMap.cpp
    src/game/map/MapFuncs.cpp
    src/game/troops/BaseTroop.cpp
    src/game/buildings/BaseBuild.cpp
    src/game/buildings/Port.cpp
    src/game/entity/EntityFuncs.cpp
    src/game/GameConfig.cpp
    src/game/Game.cpp
    src/game/GameInit.cpp
    src/game/GameUtils.cpp
    src/game/GameRender.cpp
    src/game/GameEvents.cpp 
    src/game/GameLogic.cpp
    src/game/GameMap.cpp 
    src/game/EndGame.cpp
    src/game/BotsAction.cpp

    src/textures/EmbeddedResources.cpp
)

add_executable(GameProject ${SOURCE_FILES})

target_link_libraries(GameProject PRIVATE sfml-graphics sfml-window)
