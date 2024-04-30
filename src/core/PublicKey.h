#ifndef RINGSWARM_PUBLICKEY_H
#define RINGSWARM_PUBLICKEY_H

#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/Signature.h"
#include "Id.h"
#include <array>
#include <cstdint>

namespace RingSwarm::core {
class PublicKey : public std::vector<uint8_t> {
public:
    std::shared_ptr<Id> getId();

    bool verifyData(std::vector<char> &pl, crypto::Signature *sign);
};
}// namespace RingSwarm::core

#endif//RINGSWARM_PUBLICKEY_H
