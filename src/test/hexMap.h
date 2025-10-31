#include <random>
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
