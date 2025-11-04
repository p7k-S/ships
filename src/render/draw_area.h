#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <cmath>
#include "../game/map.h"
#include "../game/detect_area.h"

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>

using namespace GameLogic;

const double SQRT3 = std::sqrt(3.0);

// === функции для координат ===
sf::Vector2f hexToPixel(int q, int r, double hexRadius) {
    double x = q * hexRadius * 1.5;
    double y = r * hexRadius * SQRT3 + (q % 2) * hexRadius * SQRT3 / 2.0;
    return sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
}

std::vector<sf::Vector2f> getHexCorners(sf::Vector2f center, double hexRadius) {
    std::vector<sf::Vector2f> corners;
    for (int i = 0; i < 6; ++i) {
        double angle_deg = 60 * i;
        double angle_rad = M_PI / 180.0 * angle_deg;
        corners.push_back({
            static_cast<float>(center.x + hexRadius * std::cos(angle_rad)),
            static_cast<float>(center.y + hexRadius * std::sin(angle_rad))
        });
    }
    return corners;
}

// === Построение линий границы ===
sf::VertexArray buildBoundaryLines(
    const std::vector<Hex*>& outerBoundary,
    const std::map<std::pair<int,int>, Hex*>& hexMap,
    double hexRadius)
{
    sf::VertexArray lines(sf::Lines);
    lines.clear();

    for (Hex* hex : outerBoundary) {
        const auto& directions = (hex->r % 2 == 0) ? DIRECTIONS_EVEN : DIRECTIONS_ODD;
        auto center = hexToPixel(hex->q, hex->r, hexRadius);
        auto corners = getHexCorners(center, hexRadius);

        for (int i = 0; i < 6; ++i) {
            int nq = hex->q + directions[i].first;
            int nr = hex->r + directions[i].second;

            // если соседа нет — рисуем эту грань
            if (hexMap.find({nq, nr}) == hexMap.end()) {
                lines.append(sf::Vertex(corners[i], sf::Color::Red));
                lines.append(sf::Vertex(corners[(i + 1) % 6], sf::Color::Red));
            }
        }
    }

    return lines;
}

void drawOuterBoundary(sf::RenderWindow& window,
                       const std::vector<Hex*>& allHexes,
                       double hexRadius)
{
    // собираем карту
    std::map<std::pair<int,int>, Hex*> hexMap;
    for (Hex* h : allHexes)
        hexMap[{h->q, h->r}] = h;

    // получаем внешние гексы
    auto outerBoundary = getOuterBoundaryHexes(allHexes);

    // строим линии
    sf::VertexArray boundaryLines = buildBoundaryLines(outerBoundary, hexMap, hexRadius);

    // немного сместим, как у тебя (+50, +50)
    for (size_t i = 0; i < boundaryLines.getVertexCount(); ++i) {
        boundaryLines[i].position.x += 50;
        boundaryLines[i].position.y += 50;
    }

    window.draw(boundaryLines);
}
