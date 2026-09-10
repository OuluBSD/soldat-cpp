#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <random>
#include <cstdint>

// Random function implementation
inline int Random(int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, max - 1);
    return dis(gen);
}

inline int Random(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

inline float Random(float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, max);
    return static_cast<float>(dis(gen));
}

#endif // MATHUTILS_H