#ifndef RINGSWARM_CHUNKLINK_H
#define RINGSWARM_CHUNKLINK_H

#include <string>
#include "Id.h"
#include "../crypto/AsymmetricalCrypto.h"

namespace RingSwarm::core {
    struct ChunkLink {
        Id *const file;
        const uint64_t chunkIndex;
        Id *const dataHash;
        crypto::Signature *const sign;

        constexpr uint32_t getSerializedSize() {
            return 32 + 8 + 32 + sign->size();
        }

        static ChunkLink *createChunkLink(Id *file,
                                          uint64_t chunkIndex,
                                          Id *dataHash);
    };
}

#endif //RINGSWARM_CHUNKLINK_H
