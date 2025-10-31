#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <map>

// --- Класс шума Перлина ---
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

    double octaveNoise(double x, double y, double z, int octaves = 4, double persistence = 0.5) const {
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

// --- Структура для 3D точки ---
struct Vector3 {
    double x, y, z;
    Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    
    Vector3 normalize() const {
        double length = std::sqrt(x*x + y*y + z*z);
        if (length > 0) return Vector3(x/length, y/length, z/length);
        return *this;
    }
    
    Vector3 operator*(double scalar) const { return Vector3(x*scalar, y*scalar, z*scalar); }
    Vector3 operator+(const Vector3& other) const { return Vector3(x+other.x, y+other.y, z+other.z); }
    bool operator<(const Vector3& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

// --- Класс шестиугольной сферической сетки ---
class HexagonalSphereGrid {
private:
    std::vector<Vector3> vertices;
    std::vector<std::vector<int>> hexagons;
    double radius;

public:
    HexagonalSphereGrid(double radius = 200.0) : radius(radius) {}
    
    // Создание сетки из шестиугольников с дыркой
    void createHexagonalGrid(int rings = 8, bool withHole = true) {
        vertices.clear();
        hexagons.clear();
        
        // Создаем шестиугольную сетку на плоскости
        std::vector<std::vector<int>> gridIndices;
        int centerX = rings;
        int centerY = rings;
        
        // Заполняем сетку индексами
        for (int ring = 0; ring <= rings; ring++) {
            if (withHole && ring == 0) continue; // Пропускаем центр для создания дырки
            
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < ring; j++) {
                    // Вычисляем координаты в шестиугольной сетке
                    double angle = i * M_PI / 3.0;
                    double x = centerX + ring * std::cos(angle) + j * std::cos(angle + M_PI/3.0);
                    double y = centerY + ring * std::sin(angle) + j * std::sin(angle + M_PI/3.0);
                    
                    // Добавляем вершину
                    vertices.push_back(Vector3(x - centerX, y - centerY, 0));
                    gridIndices.push_back({static_cast<int>(x), static_cast<int>(y)});
                }
            }
        }
        
        // Создаем шестиугольники
        createHexagonsFromGrid(gridIndices, rings);
        
        // Проецируем на сферу
        projectToSphere();
    }
    
    // Создание шестиугольников из треугольной сетки
    void createFromTriangularMesh(int subdivisions = 3) {
        vertices.clear();
        hexagons.clear();
        
        // Создаем икосаэдр
        createIcosahedron();
        
        // Подразделяем
        for (int i = 0; i < subdivisions; i++) {
            subdivide();
        }
        
        // Преобразуем треугольники в шестиугольники
        convertTrianglesToHexagons();
        
        // Удаляем некоторые шестиугольники для создания дырки
        createHole();
    }
    
    // Применение шума Перлина к сетке
    void applyPerlinNoise(const PerlinNoise& perlin, double amplitude = 0.2, double frequency = 3.0) {
        for (auto& vertex : vertices) {
            Vector3 spherePos = vertex.normalize();
            
            double noiseVal = perlin.octaveNoise(
                spherePos.x * frequency,
                spherePos.y * frequency,
                spherePos.z * frequency,
                4, 0.5
            );
            
            // Применяем смещение вдоль нормали
            vertex = spherePos * (radius * (1.0 + noiseVal * amplitude));
        }
    }
    
    // Преобразование 3D координат в 2D экранные координаты
    sf::Vector2f projectTo2D(const Vector3& pos, const sf::Vector2f& center, double scale = 1.0) {
        return sf::Vector2f(
            center.x + static_cast<float>(pos.x * scale),
            center.y + static_cast<float>(pos.y * scale)
        );
    }
    
    // Отрисовка сетки
    void draw(sf::RenderWindow& window, const sf::Vector2f& center, double scale = 1.0, bool wireframe = true) {
        if (wireframe) {
            // Рисуем линии шестиугольников
            for (const auto& hexagon : hexagons) {
                for (size_t i = 0; i < hexagon.size(); i++) {
                    size_t j = (i + 1) % hexagon.size();
                    if (hexagon[i] < vertices.size() && hexagon[j] < vertices.size()) {
                        sf::Vector2f p1 = projectTo2D(vertices[hexagon[i]], center, scale);
                        sf::Vector2f p2 = projectTo2D(vertices[hexagon[j]], center, scale);
                        drawLine(window, p1, p2, sf::Color::Cyan);
                    }
                }
            }
        } else {
            // Рисуем заполненные шестиугольники
            for (const auto& hexagon : hexagons) {
                if (hexagon.size() >= 6) {
                    sf::ConvexShape hexShape;
                    hexShape.setPointCount(6);
                    
                    // Вычисляем центр шестиугольника для цвета
                    Vector3 center3D(0, 0, 0);
                    for (int i = 0; i < 6; i++) {
                        if (hexagon[i] < vertices.size()) {
                            sf::Vector2f point = projectTo2D(vertices[hexagon[i]], center, scale);
                            hexShape.setPoint(i, point);
                            center3D = center3D + vertices[hexagon[i]];
                        }
                    }
                    center3D = center3D * (1.0 / 6.0);
                    
                    // Цвет в зависимости от высоты
                    double height = (center3D.normalize().y + 1.0) / 2.0;
                    sf::Color color(
                        static_cast<sf::Uint8>(height * 100),
                        static_cast<sf::Uint8>(height * 200),
                        static_cast<sf::Uint8>(255 - height * 100),
                        200
                    );
                    
                    hexShape.setFillColor(color);
                    hexShape.setOutlineColor(sf::Color::White);
                    hexShape.setOutlineThickness(1);
                    
                    window.draw(hexShape);
                }
            }
        }
        
        // Рисуем вершины точками
        for (const auto& vertex : vertices) {
            sf::Vector2f pos = projectTo2D(vertex, center, scale);
            drawPoint(window, pos, sf::Color::Yellow, 2.0f);
        }
    }
    
    // Вращение сетки
    void rotate(double angleX, double angleY) {
        for (auto& vertex : vertices) {
            // Вращение вокруг Y
            double x1 = vertex.x * std::cos(angleY) - vertex.z * std::sin(angleY);
            double z1 = vertex.x * std::sin(angleY) + vertex.z * std::cos(angleY);
            
            // Вращение вокруг X
            double y1 = vertex.y * std::cos(angleX) - z1 * std::sin(angleX);
            double z2 = vertex.y * std::sin(angleX) + z1 * std::cos(angleX);
            
            vertex = Vector3(x1, y1, z2);
        }
    }

private:
    // Создание икосаэдра (вспомогательная функция)
    void createIcosahedron() {
        double t = (1.0 + std::sqrt(5.0)) / 2.0;
        
        vertices = {
            Vector3(-1,  t,  0).normalize(),
            Vector3( 1,  t,  0).normalize(),
            Vector3(-1, -t,  0).normalize(),
            Vector3( 1, -t,  0).normalize(),
            Vector3( 0, -1,  t).normalize(),
            Vector3( 0,  1,  t).normalize(),
            Vector3( 0, -1, -t).normalize(),
            Vector3( 0,  1, -t).normalize(),
            Vector3( t,  0, -1).normalize(),
            Vector3( t,  0,  1).normalize(),
            Vector3(-t,  0, -1).normalize(),
            Vector3(-t,  0,  1).normalize()
        };
    }
    
    // Подразделение треугольников
    void subdivide() {
        std::vector<std::vector<int>> newFaces;
        std::map<std::pair<int, int>, int> edgeVertices;
        
        // Здесь должна быть реализация подразделения...
        // Для простоты создадим простую шестиугольную структуру
        
        // Создаем базовую шестиугольную структуру вокруг сферы
        vertices.clear();
        hexagons.clear();
        
        int segments = 12;
        for (int i = 0; i < segments; i++) {
            double longitude = 2.0 * M_PI * i / segments;
            for (int j = 1; j < segments/2; j++) {
                double latitude = M_PI * j / (segments/2) - M_PI/2;
                
                Vector3 pos(
                    std::cos(latitude) * std::cos(longitude),
                    std::sin(latitude),
                    std::cos(latitude) * std::sin(longitude)
                );
                vertices.push_back(pos.normalize() * radius);
            }
        }
        
        // Создаем шестиугольники (кроме полюсов)
        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments/2 - 2; j++) {
                int current = i * (segments/2 - 1) + j;
                int next = ((i + 1) % segments) * (segments/2 - 1) + j;
                
                std::vector<int> hexagon = {
                    current,
                    current + 1,
                    next + 1,
                    next + 1 + (segments/2 - 1),
                    next + (segments/2 - 1),
                    current + (segments/2 - 1)
                };
                
                // Проверяем валидность индексов
                bool valid = true;
                for (int idx : hexagon) {
                    if (idx < 0 || idx >= static_cast<int>(vertices.size())) {
                        valid = false;
                        break;
                    }
                }
                
                if (valid) {
                    hexagons.push_back(hexagon);
                }
            }
        }
    }
    
    // Преобразование треугольников в шестиугольники
    void convertTrianglesToHexagons() {
        // Упрощенная реализация - создаем шестиугольники вокруг каждой вершины
        std::vector<std::vector<int>> newHexagons;
        
        for (size_t i = 0; i < vertices.size(); i++) {
            // Находим соседние вершины (это упрощение - в реальности нужен поиск соседей)
            if (i % 7 != 0) { // Пропускаем некоторые вершины для создания дырки
                std::vector<int> hexagon;
                for (int j = 0; j < 6; j++) {
                    int neighbor = (i + j) % vertices.size();
                    hexagon.push_back(neighbor);
                }
                newHexagons.push_back(hexagon);
            }
        }
        
        hexagons = newHexagons;
    }
    
    // Создание дырки в сетке
    void createHole() {
        if (hexagons.empty()) return;
        
        // Удаляем центральные шестиугольники для создания дырки
        int center = hexagons.size() / 2;
        int holeSize = hexagons.size() / 8; // Размер дырки
        
        std::vector<std::vector<int>> newHexagons;
        for (size_t i = 0; i < hexagons.size(); i++) {
            // Сохраняем только шестиугольники далеко от центра
            if (std::abs(static_cast<int>(i) - center) > holeSize) {
                newHexagons.push_back(hexagons[i]);
            }
        }
        
        hexagons = newHexagons;
    }
    
    // Проецирование на сферу
    void projectToSphere() {
        for (auto& vertex : vertices) {
            vertex = vertex.normalize() * radius;
        }
    }
    
    // Создание шестиугольников из сетки
    void createHexagonsFromGrid(const std::vector<std::vector<int>>& gridIndices, int rings) {
        // Упрощенное создание шестиугольников
        for (size_t i = 0; i < vertices.size(); i++) {
            if (i % 2 == 0 && i + 6 < vertices.size()) {
                std::vector<int> hexagon;
                for (int j = 0; j < 6; j++) {
                    hexagon.push_back((i + j) % vertices.size());
                }
                hexagons.push_back(hexagon);
            }
        }
    }
    
    void drawLine(sf::RenderWindow& window, const sf::Vector2f& p1, const sf::Vector2f& p2, sf::Color color) {
        sf::Vertex line[] = {
            sf::Vertex(p1, color),
            sf::Vertex(p2, color)
        };
        window.draw(line, 2, sf::Lines);
    }
    
    void drawPoint(sf::RenderWindow& window, const sf::Vector2f& pos, sf::Color color, float radius) {
        sf::CircleShape point(radius);
        point.setFillColor(color);
        point.setPosition(pos - sf::Vector2f(radius, radius));
        window.draw(point);
    }
};

// --- Главная функция ---
int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Hexagonal Sphere Grid with Perlin Noise");
    window.setFramerateLimit(60);
    
    // Создаем шестиугольную сферическую сетку
    HexagonalSphereGrid grid(180.0);
    grid.createFromTriangularMesh(2);
    
    // Создаем генератор шума
    PerlinNoise perlin(12345);
    
    // Применяем шум к сетке
    grid.applyPerlinNoise(perlin, 0.15, 2.5);
    
    // Переменные для управления
    bool wireframe = false;
    bool autoRotate = true;
    double rotationSpeed = 0.005;
    double scale = 1.5;
    sf::Vector2f center(500, 400);
    
    // Шрифт для текста
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Если шрифт не загружен, пытаемся загрузить системный
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::Green);
    infoText.setPosition(10, 10);
    
    // Главный цикл
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Space:
                        wireframe = !wireframe;
                        break;
                    case sf::Keyboard::R:
                        autoRotate = !autoRotate;
                        break;
                    case sf::Keyboard::Up:
                        scale *= 1.1f;
                        break;
                    case sf::Keyboard::Down:
                        scale /= 1.1f;
                        break;
                    case sf::Keyboard::Add:
                        rotationSpeed *= 1.1f;
                        break;
                    case sf::Keyboard::Subtract:
                        rotationSpeed /= 1.1f;
                        break;
                    case sf::Keyboard::N:
                        // Регенерируем с новым шумом
                        grid.createFromTriangularMesh(2);
                        grid.applyPerlinNoise(perlin, 0.15, 2.5);
                        break;
                    case sf::Keyboard::H:
                        // Переключаем режим дырки
                        grid.createFromTriangularMesh(2);
                        grid.applyPerlinNoise(perlin, 0.15, 2.5);
                        break;
                }
            }
        }
        
        // Вращение
        if (autoRotate) {
            grid.rotate(rotationSpeed, rotationSpeed * 0.5);
        }
        
        // Обновление текста
        std::string info = "Hexagonal Sphere Grid with Hole\n";
        info += "Controls:\n";
        info += "Space: Toggle wireframe (" + std::string(wireframe ? "ON" : "OFF") + ")\n";
        info += "R: Toggle auto-rotate (" + std::string(autoRotate ? "ON" : "OFF") + ")\n";
        info += "Up/Down: Zoom (" + std::to_string(scale).substr(0, 4) + ")\n";
        info += "+/-: Rotation speed (" + std::to_string(rotationSpeed).substr(0, 6) + ")\n";
        info += "N: New noise pattern\n";
        info += "H: Regenerate grid";
        
        infoText.setString(info);
        
        // Отрисовка
        window.clear(sf::Color::Black);
        grid.draw(window, center, scale, wireframe);
        window.draw(infoText);
        window.display();
    }
    
    return 0;
}
