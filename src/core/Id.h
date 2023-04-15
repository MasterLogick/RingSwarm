#ifndef RINGSWARM_ID_H
#define RINGSWARM_ID_H

#include <cstdint>

namespace RingSwarm::core {
    struct Id {
        uint8_t hash[32];

        bool operator==(const Id &rhs) const;

        bool operator!=(const Id &rhs) const;
    };
}

#endif //RINGSWARM_ID_H
