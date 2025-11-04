cmake_minimum_required(VERSION 3.10)

# Установим имя проекта
project(GameProject)

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите путь к SFML (если она не установлена глобально, или у вас есть её локальная копия)
# Например, если вы используете систему на Linux, может потребоваться указать путь к SFML:
# set(SFML_DIR "/path/to/sfml/share/SFML/cmake")

# Найдите SFML
find_package(SFML 2.5 REQUIRED graphics window system)

# Укажите директории, которые содержат заголовочные файлы
include_directories(
    ${CMAKE_SOURCE_DIR}/src/game
    ${CMAKE_SOURCE_DIR}/src/render
    ${CMAKE_SOURCE_DIR}/src/render/map
)

# Укажите исходные файлы
set(SOURCE_FILES
    src/main.cpp
    src/game/GameLogic.h
    src/game/map.h
    src/game/ship.h
    src/game/ship.cpp
    src/render/Colors.hpp
    src/game/CubePerlin.h
    src/render/map/MapReneder.h
    src/game/detect_area.h
    src/render/draw_area.h
)

# Укажите исполняемый файл, который будет скомпилирован
add_executable(GameProject ${SOURCE_FILES})

# Линкуем SFML с проектом
target_link_libraries(GameProject PRIVATE sfml-graphics sfml-window sfml-system)

# Если нужно, можно указать дополнительные флаги или пути для поиска библиотек
# Например, если вам нужно использовать статические или динамические библиотеки SFML, укажите это:
# target_link_libraries(GameProject PRIVATE sfml-graphics sfml-window sfml-system)

