#ifndef RINGSWARM_PUBLICKEY_H
#define RINGSWARM_PUBLICKEY_H

#include <array>
#include <cstdint>
#include "Id.h"

namespace RingSwarm::core {
    class PublicKey : public std::array<uint8_t, 33> {
    public:
        Id *getId();
    };
}

#endif //RINGSWARM_PUBLICKEY_H
