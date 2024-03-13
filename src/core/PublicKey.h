#ifndef RINGSWARM_PUBLICKEY_H
#define RINGSWARM_PUBLICKEY_H

#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/Signature.h"
#include "Id.h"
#include <array>
#include <cstdint>

namespace RingSwarm::core {
class PublicKey : public std::array<uint8_t, 33> {
public:
    Id *getId();

    bool verifyData(std::vector<char> &data, crypto::Signature *sign);
};
}// namespace RingSwarm::core

#endif//RINGSWARM_PUBLICKEY_H
