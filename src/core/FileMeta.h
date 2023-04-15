#ifndef RINGSWARM_FILEMETA_H
#define RINGSWARM_FILEMETA_H

#include <cstdint>
#include <string>
#include "Id.h"

namespace RingSwarm::core {
    class FileMeta {
        Id *author;
        uint64_t creationTimestamp;
        uint64_t chunksCount;
        uint32_t chunkSize;
        uint8_t minSwarmSize;
        uint8_t ringConnectivity;
        uint8_t optMeta[10];
        std::string sign;
        Id *fileId;
    public:
        FileMeta();

        int getSerializedSize();

        Id *getId();
    };
}

#endif //RINGSWARM_FILEMETA_H
