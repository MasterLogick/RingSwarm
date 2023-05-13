#ifndef RINGSWARM_CHUNKLINK_H
#define RINGSWARM_CHUNKLINK_H

#include <string>
#include "Id.h"

namespace RingSwarm::core {
    struct ChunkLink {
        Id *const file;
        const uint64_t chunkIndex;
        Id *const dataHash;
        const std::vector<char> sign;

        uint32_t getSerializedSize();

        static ChunkLink *createChunkLink(Id *file,
                                          uint64_t chunkIndex,
                                          Id *dataHash);
    };
}

#endif //RINGSWARM_CHUNKLINK_H
