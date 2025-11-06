#pragma once

#include <string>

const int treasures = 3; // от размеров карты автоматически

const double percent_ships_in_water = 0.02;

const double deepWater_delim = 0.2;  // 0.2
const double water_delim = 0.65; // 0.65
const double land_delim = 0.9; // 0.9

static int mapWidth = 24;
static int mapHeight = 15;
static double scale = 0.1;
static double persistance = 0.5;
static int seed = 0;
static int octaves = 1;
static bool random_map = false;
static bool unknown_map = false;
static double hexRadius = 30.0;

static int font_size = 10;
static std::string font_path = "/home/zane/Study/mirea/sem3/kursach/src/textures/JetBrainsMonoNLNerdFont-Regular.ttf";
