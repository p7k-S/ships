#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>

// --- Перлин шум 3D ---
class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = std::random_device{}()) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::mt19937 generator(seed);
        std::shuffle(p.begin(), p.end(), generator);
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y, double z) const {
        int xi = (int)std::floor(x) & 255;
        int yi = (int)std::floor(y) & 255;
        int zi = (int)std::floor(z) & 255;

        double xf = x - std::floor(x);
        double yf = y - std::floor(y);
        double zf = z - std::floor(z);

        double u = fade(xf);
        double v = fade(yf);
        double w = fade(zf);

        int aaa = p[p[p[xi] + yi] + zi];
        int aab = p[p[p[xi] + yi] + zi + 1];
        int aba = p[p[p[xi] + yi + 1] + zi];
        int abb = p[p[p[xi] + yi + 1] + zi + 1];
        int baa = p[p[p[xi + 1] + yi] + zi];
        int bab = p[p[p[xi + 1] + yi] + zi + 1];
        int bba = p[p[p[xi + 1] + yi + 1] + zi];
        int bbb = p[p[p[xi + 1] + yi + 1] + zi + 1];

        double x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
        double x2 = lerp(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
        double y1 = lerp(x1, x2, v);

        double x3 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
        double x4 = lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);
        double y2 = lerp(x3, x4, v);

        return (lerp(y1, y2, w) + 1.0) / 2.0;
    }

    double octaveNoise(double x, double y, double z, int octaves, double persistence = 0.5) const {
        double total = 0.0;
        double frequency = 1.0;
        double amplitude = 1.0;
        double maxValue = 0.0;

        for (int i = 0; i < octaves; i++) {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0;
        }
        return total / maxValue;
    }

private:
    std::vector<int> p;

    static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    static double lerp(double a, double b, double t) { return a + t * (b - a); }

    static double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
};

// --- Гекс ---
struct Hex { int q, r; };

// --- Функция создания гекса для SFML ---
sf::ConvexShape createHex(double x, double y, double radius) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3.0 * i;
        double px = x + radius * std::cos(angle);
        double py = y + radius * std::sin(angle);
        hex.setPoint(i, sf::Vector2f(px, py));
    }
    return hex;
}

// --- Перевод гекса в кубические координаты ---
void axialToCube(int q, int r, int& x, int& y, int& z) {
    x = q;
    z = r;
    y = -x - z;
}

enum ColorScheme {
    COLORS,
    DEFAULT
};
ColorScheme currentScheme = COLORS;

sf::Color getColorByThreshold(double value,
                              double deepWater,
                              double water,
                              double land) {
    if (value <= deepWater)
        return sf::Color(0x0245ABFF);  // sf::Color(2, 61, 171)
    else if (value <= water)
        return sf::Color(0x1E5AC8FF);  // sf::Color(30, 90, 200)
    else if (value <= land)
        return sf::Color(0xA08C5AFF);  // sf::Color(160, 140, 90)
    else
        return sf::Color(0x2D6441FF);  // sf::Color(45, 100, 65)
}

sf::Color getGrayscaleColor(double value) {
    int colorValue = static_cast<int>(value * 255);
    return sf::Color(colorValue, colorValue, colorValue);
}

sf::Color getColorByScheme(double value, ColorScheme scheme, double deepWater, double water, double land) {
    switch(scheme) {
        case DEFAULT:
            return getGrayscaleColor(value);
        case COLORS:
        default:
            return getColorByThreshold(value, deepWater, water, land);
    }
}

int main() {
    const int mapWidth= 25;
    const int mapHeight = 15;
    const double hexRadius = 30.0;

    int seed, octaves;
    std::cout << "input seed: ";
    std::cin >> seed;
    std::cout << "input octaves (not too much): ";
    std::cin >> octaves;
    // 1
    // 2

    PerlinNoise perlin(seed);

    std::vector<Hex> hexMap;
    for (int r = 0; r < mapHeight; r++) {
        for (int q = 0; q < mapWidth; q++) {
            hexMap.push_back({q, r});
        }
    }
    std::vector<double> noiseValues;
    for (const auto& hex : hexMap) {
        int cx, cy, cz;
        axialToCube(hex.q, hex.r, cx, cy, cz);
        double value = perlin.octaveNoise(cx * 0.1, cy * 0.1, cz * 0.1, octaves, 0.5);
        noiseValues.push_back(value);
    }

    std::vector<double> sortedValues = noiseValues;
    std::sort(sortedValues.begin(), sortedValues.end());
    double deepWater = sortedValues[noiseValues.size() * 0.2];
    double water = sortedValues[noiseValues.size() * 0.65];
    double land = sortedValues[noiseValues.size() * 0.9];


    sf::RenderWindow window(sf::VideoMode(800, 600), "Hex Map 3D Perlin");
    sf::View view = window.getDefaultView(); // Get the default view

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    view.zoom(0.9f); // Zoom in
                } else {
                    view.zoom(1.1f); // Zoom out
                }
                window.setView(view); // Apply the new view
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                currentScheme = (currentScheme == COLORS) ? DEFAULT : COLORS;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                    view.move(-10.f, 0.f); // Move left
                if (event.key.code == sf::Keyboard::Right)
                    view.move(10.f, 0.f);  // Move right
                if (event.key.code == sf::Keyboard::Up)
                    view.move(0.f, -10.f); // Move up
                if (event.key.code == sf::Keyboard::Down)
                    view.move(0.f, 10.f);  // Move down
                window.setView(view); // Apply the updated view
            }
        }

        window.clear(sf::Color::Black);

        for (const auto& hex : hexMap) {
            double x_pos = hex.q * hexRadius * 1.5;
            double y_pos = hex.r * hexRadius * sqrt(3) + (hex.q % 2) * hexRadius * sqrt(3) / 2.0;

            int cx, cy, cz;
            axialToCube(hex.q, hex.r, cx, cy, cz);
            double value = perlin.octaveNoise(cx * 0.1, cy * 0.1, cz * 0.1, octaves, 0.5);

            sf::ConvexShape hexShape = createHex(x_pos + 50, y_pos + 50, hexRadius);

            hexShape.setFillColor(getColorByScheme(value, currentScheme, deepWater, water, land));

            hexShape.setOutlineColor(sf::Color::Black);
            hexShape.setOutlineThickness(1.0f);

            window.draw(hexShape);
        }

        window.display();
    }

    return 0;
}
