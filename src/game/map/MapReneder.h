#pragma once
#include <SFML/Graphics.hpp>
#include <charconv>
#include <cmath>
#include "Cell.h"
#include "PerlinNoise.h"

sf::ConvexShape createHex(double x, double y, double radius);

std::vector<GameLogic::Hex> createMap(const PerlinNoise& perlin, int mapWidth, int mapHeight, int octaves, const double scale);

std::vector<double> getNoises(std::vector<GameLogic::Hex> hexMap);
