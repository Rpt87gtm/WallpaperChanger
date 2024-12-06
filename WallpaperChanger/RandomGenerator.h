#include <random>
#include "IRandomGenerator.h"
class RandomGenerator : public IRandomGenerator {
public:
    RandomGenerator();

    int GetRandomIndex(int maxIndex) override;

protected:
    std::random_device rd;
    std::mt19937 gen;
};

