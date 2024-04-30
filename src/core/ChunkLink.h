#ifndef RINGSWARM_CHUNKLINK_H
#define RINGSWARM_CHUNKLINK_H

#include "../crypto/AsymmetricalCrypto.h"
#include "Id.h"
#include <string>

namespace RingSwarm::core {
struct ChunkLink {
    std::shared_ptr<Id> const keyId;
    const uint64_t chunkIndex;
    std::shared_ptr<Id> const dataHash;
    const uint64_t dataSize;
    crypto::Signature *const sign;

    constexpr uint32_t getSerializedSize() {
        return 32 + 8 + 32 + 8 + sign->size();
    }

    static ChunkLink *createChunkLink(
            crypto::KeyPair &keyPair,
            uint64_t chunkIndex,
            std::shared_ptr<Id> dataHash,
            uint64_t dataSize);
};
}// namespace RingSwarm::core

#endif//RINGSWARM_CHUNKLINK_H
