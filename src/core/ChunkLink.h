#ifndef RINGSWARM_CHUNKLINK_H
#define RINGSWARM_CHUNKLINK_H

#include <string>
#include "Id.h"
#include "../crypto/AsymmetricalCrypto.h"

namespace RingSwarm::core {
    struct ChunkLink {
        Id *const keyId;
        const uint64_t chunkIndex;
        Id *const dataHash;
        const uint64_t dataSize;
        crypto::Signature *const sign;

        constexpr uint32_t getSerializedSize() {
            return 32 + 8 + 32 + 8 + sign->size();
        }

        static ChunkLink *createChunkLink(
                crypto::KeyPair &keyPair,
                uint64_t chunkIndex,
                Id *dataHash,
                uint64_t dataSize
        );
    };
}

#endif //RINGSWARM_CHUNKLINK_H
