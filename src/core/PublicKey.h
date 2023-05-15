#ifndef RINGSWARM_PUBLICKEY_H
#define RINGSWARM_PUBLICKEY_H

#include <array>
#include <cstdint>
#include "Id.h"
#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/Signature.h"

namespace RingSwarm::core {
    class PublicKey : public std::array<uint8_t, 33> {
    public:
        Id *getId();

        bool verifyData(std::vector<char> &data, crypto::Signature *sign);
    };
}

#endif //RINGSWARM_PUBLICKEY_H
