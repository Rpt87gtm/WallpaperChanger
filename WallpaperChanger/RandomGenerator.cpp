#include "RandomGenerator.h"
#include <random>
RandomGenerator::RandomGenerator() : rd(), gen(rd()) {}

int RandomGenerator::GetRandomIndex(int maxIndex) {
    std::uniform_int_distribution<> dis(0, maxIndex);
    return dis(gen);
}
