#ifndef RINGSWARM_FILEMETA_H
#define RINGSWARM_FILEMETA_H

#include <cstdint>
#include <vector>
#include "Id.h"

namespace RingSwarm::core {
    struct FileMeta {
        uint64_t internalStorageId;
        Id *author;
        uint64_t creationTimestamp;
        uint64_t chunksCount;
        uint32_t chunkSize;
        uint8_t minSwarmSize;
        uint8_t ringConnectivity;
        uint8_t optMeta[10];
        std::vector<char> sign;
        Id *fileId;

        int getSerializedSize();

        Id *getId();
    };
}

#endif //RINGSWARM_FILEMETA_H
