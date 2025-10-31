// main.cpp
#include <SFML/Graphics.hpp>
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cmath>
#include <iostream>

// --- PerlinNoise (взято из вашего кода) ---
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

// -------------------- Утилиты для сетки шестигранников --------------------
const double PI = 3.14159265358979323846;

// Создаёт sf::ConvexShape шестигранника (flat-top) с центром в (cx, cy)
// r = расстояние от центра до вершины ( радиус )
sf::ConvexShape makeHex(double cx, double cy, double r) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    // flat-top orientation: angles 0,60,120,...
    for (int i = 0; i < 6; ++i) {
        double angle = PI / 180.0 * (60.0 * i + 0.0);
        double vx = cx + r * std::cos(angle);
        double vy = cy + r * std::sin(angle);
        hex.setPoint(i, sf::Vector2f((float)vx, (float)vy));
    }
    return hex;
}

// Линейная интерполяция цвета по значению v в [0,1] -> цвет
sf::Color valueToColor(double v) {
    // простая градация: от синего (мало) -> зелёного -> коричневого -> белого (высоко)
    // делаем «ручную» интерполяцию
    v = std::clamp(v, 0.0, 1.0);
    if (v < 0.25) {
        double t = v / 0.25;
        // синий -> зелёный
        return sf::Color(
            (sf::Uint8)(0 + t * (34 - 0)),
            (sf::Uint8)(0 + t * (139 - 0)),
            (sf::Uint8)(139 + t * (34 - 139))
        );
    } else if (v < 0.5) {
        double t = (v - 0.25) / 0.25;
        // зелёный -> жёлтовато-коричневый
        return sf::Color(
            (sf::Uint8)(34 + t * (210 - 34)),
            (sf::Uint8)(139 + t * (180 - 139)),
            (sf::Uint8)(34 + t * (80 - 34))
        );
    } else if (v < 0.75) {
        double t = (v - 0.5) / 0.25;
        // коричневый -> светло-коричневый
        return sf::Color(
            (sf::Uint8)(210 + t * (222 - 210)),
            (sf::Uint8)(180 + t * (180 - 180)),
            (sf::Uint8)(80 + t * (150 - 80))
        );
    } else {
        double t = (v - 0.75) / 0.25;
        // светло-коричневый -> белый (пик)
        return sf::Color(
            (sf::Uint8)(222 + t * (255 - 222)),
            (sf::Uint8)(180 + t * (255 - 180)),
            (sf::Uint8)(150 + t * (255 - 150))
        );
    }
}

// -------------------- main --------------------
int main() {
    const unsigned int windowW = 1200;
    const unsigned int windowH = 600;
    sf::RenderWindow window(sf::VideoMode(windowW, windowH), "Perlin on sphere - hex equirectangular (SFML)");
    window.setFramerateLimit(60);

    // Параметры шестигранной сетки
    const double hexRadius = 8.0; // радиус шестиугольника в пикселях
    const double hexWidth = 2.0 * hexRadius;
    const double hexHeight = std::sqrt(3.0) * hexRadius;
    const double horiz = 0.75 * hexWidth; // горизонтальный шаг (flat-top)
    const double vert = hexHeight;

    // Сколько колонок и строк поместится
    const int cols = (int)std::ceil(windowW / horiz) + 2;
    const int rows = (int)std::ceil(windowH / (vert)) + 2;

    // Перлин
    PerlinNoise perlin((unsigned int)12345); // можно менять seed
    const double noiseScale = 2.5; // масштаб шума на сфере
    const int octaves = 4;
    const double persistence = 0.5;

    // Предвычислим все шестиугольники (их центры и shape), а также шум
    struct HexCell {
        sf::ConvexShape shape;
        double centerX;
        double centerY;
        double noiseValue;
    };
    std::vector<HexCell> cells;
    cells.reserve(cols * rows);

    // Для каждой позиционной сетки вычислим центр, сдвиг по колонке
    for (int col = -1; col < cols+1; ++col) {
        for (int row = -1; row < rows+1; ++row) {
            double cx = col * horiz;
            double cy = row * vert + ( (col & 1) ? (vert * 0.5) : 0.0 );
            // Сдвинуть в центр окна
            cx += 0;
            cy += 0;
            // фильтруем те, что в окне (или близко)
            if (cx < -hexWidth || cx > windowW + hexWidth || cy < -hexHeight || cy > windowH + hexHeight) continue;

            sf::ConvexShape hex = makeHex(cx, cy, hexRadius);
            hex.setOutlineThickness(0.5f);
            hex.setOutlineColor(sf::Color(0,0,0,80));
            HexCell h{hex, cx, cy, 0.0};
            cells.push_back(h);
        }
    }

    // Вычисление шума для каждой клетки: центр (pixel) -> lon/lat -> xyz -> perlin
    auto computeNoiseForCells = [&]() {
        for (auto &c : cells) {
            // Нормализуем координаты пикселя в диапазон [0,1]
            double u = c.centerX / (double)windowW; // 0..1
            double v = c.centerY / (double)windowH; // 0..1 (y сверху вниз)

            // Долгота: -pi .. +pi
            double lon = (u * 2.0 * PI) - PI;
            // Широта: +pi/2 (top) .. -pi/2 (bottom)  => инвертируем v
            double lat = ((1.0 - v) * PI) - PI/2.0;

            // Перевод в 3D координаты на единичной сфере
            double x = std::cos(lat) * std::cos(lon);
            double y = std::cos(lat) * std::sin(lon);
            double z = std::sin(lat);

            // Берём октавный перлин в точке (x*scale, y*scale, z*scale)
            double val = perlin.octaveNoise(x * noiseScale, y * noiseScale, z * noiseScale, octaves, persistence);
            // val уже в [0,1] по реализации octaveNoise
            c.noiseValue = val;
            // Задаём цвет фигуры
            sf::Color colr = valueToColor(val);
            c.shape.setFillColor(colr);
        }
    };

    computeNoiseForCells();

    // GUI: несколько параметров (можно менять клавишами)
    bool redraw = true;
    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();

            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) window.close();

                // интерактивно менять параметры
                if (ev.key.code == sf::Keyboard::Up) {
                    // увеличить масштаб шума (меньшие детали)
                    // уменьшаем noiseScale -> больше "размытия"
                    // но тут инверсия: увеличим noiseScale для более плотного шума
                    // (простая логика для примера)
                    // NB: не менять perlin, просто менять noiseScale не доступно здесь (const)
                }
                if (ev.key.code == sf::Keyboard::R) {
                    // сменить seed -> пересчитать
                    PerlinNoise newPerlin((unsigned int)std::random_device{}());
                    perlin = newPerlin;
                    computeNoiseForCells();
                    redraw = true;
                }
            }

            if (ev.type == sf::Event::Resized) {
                // размер окна изменился — но мы не приспособили пересчёт сетки в этом примере.
                // Для простоты — просто ограничиваемся фиксированным размером окна.
            }
        }

        if (redraw) {
            window.clear(sf::Color::Black);
            // Draw a background rectangle (ocean)
            sf::RectangleShape bg(sf::Vector2f((float)windowW, (float)windowH));
            bg.setPosition(0,0);
            bg.setFillColor(sf::Color(10, 20, 40));
            window.draw(bg);

            // Draw all hexes
            for (auto &c : cells) {
                window.draw(c.shape);
            }

            // Overlay: instructions
            sf::Font font;
            bool fontLoaded = font.loadFromFile("Arial.ttf"); // попытка загрузить; если нет — текст не покажем
            if (fontLoaded) {
                sf::Text t;
                t.setFont(font);
                t.setCharacterSize(14);
                t.setFillColor(sf::Color::Black);
                t.setPosition(8, 8);
                t.setString("Perlin on sphere (hex map). Press R to reseed.\nNote: projection = equirectangular (lon/lat).");
                window.draw(t);
            }

            window.display();
            redraw = false;
        }

        // небольшая пауза, но loop управляется framerateLimit
    }

    return 0;
}

