#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <SFML/Graphics/Color.hpp>

// main menu settings
extern uint8_t playersAmount;
extern uint16_t mapWidth;
extern uint16_t mapHeight;
extern double scale;
extern double persistance;
extern int seed;
extern uint8_t octaves;
extern bool random_map;
extern bool unknown_map;
extern double hexRadius;

extern uint8_t font_size;

extern uint16_t treasuresAmount;
extern double gold_perc_in_map;
extern double percent_ships_in_water;
extern double deepWater_delim;
extern double water_delim;
extern double land_delim;

// Colors
extern std::unordered_map<std::string, sf::Color> COLORS;
enum ColorScheme {
    COLORFULL,
    DARK_COLORS,
    INVERT
};
extern ColorScheme colSchemeDefault;
extern ColorScheme colSchemeInactive;
