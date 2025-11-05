#pragma once
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <cmath>

// --2D--
class PerlinNoise {
public:
    explicit PerlinNoise(unsigned int seed = std::random_device{}()) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::mt19937 generator(seed);
        std::shuffle(p.begin(), p.end(), generator);
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y) const {
        int xi = static_cast<int>(std::floor(x)) & 255;
        int yi = static_cast<int>(std::floor(y)) & 255;

        double xf = x - std::floor(x);
        double yf = y - std::floor(y);

        double u = fade(xf);
        double v = fade(yf);

        int aa = p[p[xi] + yi];
        int ab = p[p[xi] + yi + 1];
        int ba = p[p[xi + 1] + yi];
        int bb = p[p[xi + 1] + yi + 1];

        double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
        double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

        return (lerp(x1, x2, v) + 1.0) / 2.0;
    }

    double octaveNoise(double x, double y, int octaves, double persistence = 0.5) const {
        double total = 0.0;
        double frequency = 1.0;
        double amplitude = 1.0;
        double maxValue = 0.0;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency) * amplitude;
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

    static double grad(int hash, double x, double y) {
        int h = hash & 7; // 8 directions
        double u = h < 4 ? x : y;
        double v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
};
