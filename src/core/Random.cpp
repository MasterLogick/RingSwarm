#include "Random.h"

namespace RingSwarm::core {
    std::mt19937 *random = nullptr;

    std::mt19937 *getRandom() {
        if (random == nullptr) {
            std::random_device dev;
            random = new std::mt19937(dev());
        }
        return random;
    }
}