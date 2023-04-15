#ifndef RINGSWARM_CHUNKLINK_H
#define RINGSWARM_CHUNKLINK_H

#include <string>
#include "Id.h"

namespace RingSwarm::core {
    struct ChunkLink {
        const Id &file;
        const uint64_t index;
        const Id &dataHash;
        const std::string sign;

        uint32_t getSerializedSize();
    };
}

#endif //RINGSWARM_CHUNKLINK_H
