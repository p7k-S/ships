cmake_minimum_required(VERSION 3.10)

project(GameProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED graphics window system network)

# # Автоматическая конвертация ресурсов в один файл
# add_custom_command(
#     OUTPUT src/textures/embedded_resources.h
#     COMMAND rm -f src/textures/embedded_resources.h
#     COMMAND echo "// Embedded resources - generated automatically" > src/textures/embedded_resources.h
#     COMMAND echo "#pragma once" >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#
#     COMMAND xxd -i src/textures/player_ship.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#     COMMAND xxd -i src/textures/player_soldier.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#
#     COMMAND xxd -i src/textures/pirates_ship.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#
#     COMMAND xxd -i src/textures/enemy_ship.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#     COMMAND xxd -i src/textures/enemy_soldier.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#
#     COMMAND xxd -i src/textures/port.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#     COMMAND xxd -i src/textures/gold_cons.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#     COMMAND xxd -i src/textures/treasure.png | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     COMMAND echo "" >> src/textures/embedded_resources.h
#     COMMAND xxd -i src/textures/JetBrainsMonoNLNerdFont-Regular.ttf | sed 's/src_textures_//g' >> src/textures/embedded_resources.h
#     DEPENDS 
#         src/textures/player_ship.png
#         src/textures/pirates_ship.png
#         src/textures/enemy_ship.png
#         src/textures/gold_cons.png
#         src/textures/treasure.png
#         src/textures/JetBrainsMonoNLNerdFont-Regular.ttf
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#     COMMENT "Creating embedded_resources.h with all resources"
# )
#
# # Отдельная команда для генерации UIfont.h
# add_custom_command(
#     OUTPUT src/textures/UIfont.h
#     COMMAND rm -f src/textures/UIfont.h
#     COMMAND echo "// UI font - generated automatically" > src/textures/UIfont.h
#     COMMAND echo "#pragma once" >> src/textures/UIfont.h
#     COMMAND echo "" >> src/textures/UIfont.h
#     COMMAND xxd -i src/textures/airborne.ttf | sed 's/src_textures_//g' >> src/textures/UIfont.h
#     DEPENDS 
#         src/textures/airborne.ttf
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#     COMMENT "Creating UIfont.h with airborne font"
# )

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
    ${CMAKE_SOURCE_DIR}/src/net
)

set(SOURCE_FILES
    src/main.cpp
    src/game/map/GenerateMap.cpp
    src/game/map/MapFuncs.cpp
    src/game/troops/BaseTroop.cpp
    src/game/buildings/BaseBuild.cpp
    src/game/entity/EntityFuncs.cpp
    src/game/GameConfig.cpp
    src/game/Game.cpp
    src/game/GameInit.cpp
    src/game/GameRender.cpp
    src/game/GameEvents.cpp 
    src/game/GameLogic.cpp
    src/game/GameMap.cpp 
    src/game/GameUtils.cpp

    src/render/ui/UIRenderer.cpp
    src/textures/EmbeddedResources.cpp
    # net
    src/net/Client.cpp
    src/net/Server.cpp
)

add_executable(GameProject ${SOURCE_FILES})
# add_dependencies(GameProject embedded_resources)

target_link_libraries(GameProject PRIVATE sfml-graphics sfml-window sfml-system sfml-network)
