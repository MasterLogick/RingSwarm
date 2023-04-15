#ifndef RINGSWARM_RANDOM_H
#define RINGSWARM_RANDOM_H

#include <random>

namespace RingSwarm::core {
    std::mt19937 *getRandom();

    template<typename T>
    T getRandom(T range) {
        std::uniform_int_distribution<T> dist(0, range - 1);
        return dist(*getRandom());
    }
}

#endif //RINGSWARM_RANDOM_H
