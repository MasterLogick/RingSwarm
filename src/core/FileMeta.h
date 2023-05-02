#ifndef RINGSWARM_FILEMETA_H
#define RINGSWARM_FILEMETA_H

#include <cstdint>
#include <vector>
#include "Id.h"

namespace RingSwarm::core {
    struct FileMeta {
        Id *const author;
        const uint64_t creationTimestamp;
        const uint64_t chunksCount;
        const uint32_t chunkSize;
        const uint8_t minSwarmSize;
        const uint8_t ringConnectivity;
        const uint8_t optMeta[10];
        const std::vector<char> sign;
        Id *const fileId;

        int getSerializedSize();
    };
}

#endif //RINGSWARM_FILEMETA_H
