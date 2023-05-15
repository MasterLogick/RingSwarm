#ifndef RINGSWARM_SIGNATURE_H
#define RINGSWARM_SIGNATURE_H

#include <array>
#include <cstdint>

namespace RingSwarm::crypto {
    class Signature : public std::array<uint8_t, 72> {

    };
}

#endif //RINGSWARM_SIGNATURE_H
