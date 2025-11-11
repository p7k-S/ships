#include "GameConfig.h"

uint8_t playersAmount = 1;
uint16_t mapWidth = 24;
uint16_t mapHeight = 15;
double scale = 0.1;
double persistance = 0.5;
int seed = 0;
uint8_t octaves = 1;
bool random_map = false;
bool unknown_map = false;
double hexRadius = 30.0;

uint16_t treasuresAmount = 3;
double gold_perc_in_map = 0.15;
double percent_ships_in_water = 0.02;
double deepWater_delim = 0.2;
double water_delim = 0.65;
double land_delim = 0.9;

uint8_t font_size = 10;
std::string font_path = "../src/textures/JetBrainsMonoNLNerdFont-Regular.ttf";

std::unordered_map<std::string, sf::Color> COLORS = {
    // Players color
    {"red", sf::Color(255, 0, 0)},
    {"green", sf::Color(0, 255, 0)},
    {"blue", sf::Color(0, 0, 255)},
    {"yellow", sf::Color(255, 255, 0)},
    {"magenta", sf::Color(255, 0, 255)},
    {"cyan", sf::Color(0, 255, 255)},
    {"orange", sf::Color(255, 165, 0)},
    {"purple", sf::Color(128, 0, 128)},
    {"pink", sf::Color(255, 192, 203)},
    {"lime", sf::Color(50, 205, 50)},
    {"turquoise", sf::Color(64, 224, 208)},
    {"gold", sf::Color(255, 215, 0)},
    {"coral", sf::Color(255, 127, 80)},
    {"violet", sf::Color(238, 130, 238)},
    {"sky_blue", sf::Color(135, 206, 235)},
    {"hot_pink", sf::Color(255, 20, 147)},
    {"electric_blue", sf::Color(125, 249, 255)},
    {"neon_green", sf::Color(57, 255, 20)},

    // entity colors
    {"enemy", sf::Color(255, 0, 0)}, // red
    {"pirate", sf::Color(0, 0, 0)}, // black
    
    // Other colors
    {"gray", sf::Color(128, 128, 128)},
    {"dark_gray", sf::Color(80, 80, 80)},
    {"deep_yellow", sf::Color(255, 200, 50)},
    {"burgundy", sf::Color(150, 30, 70)},
    {"dark_green", sf::Color(2, 124, 2)},
    {"reachable_border", sf::Color(2, 124, 2)}, // borders of reachable cells
    {"white", sf::Color(255, 255, 255)},
    {"very_dark_gray", sf::Color(40, 40, 40)},
    {"yellow2", sf::Color(255, 255, 0)},
    {"dark_red", sf::Color(128, 0, 0)},
    {"dark_green2", sf::Color(0, 128, 0)}
};

ColorScheme colSchemeDefault = COLORFULL;
ColorScheme colSchemeInactive = DARK_COLORS;
