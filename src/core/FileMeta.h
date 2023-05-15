#ifndef RINGSWARM_FILEMETA_H
#define RINGSWARM_FILEMETA_H

#include <cstdint>
#include <vector>
#include "Id.h"
#include "../crypto/AsymmetricalCrypto.h"
#include <array>

namespace RingSwarm::core {
    struct FileMeta {
        Id *const author;
        const uint64_t creationTimestamp;
        const uint64_t chunksCount;
        const uint32_t chunkSize;
        const uint8_t minSwarmSize;
        const uint8_t ringConnectivity;
        crypto::Signature *const sign;
        Id *const fileId;

        constexpr int getSerializedSize() {
            return 32 + 32 + 8 + 8 + 4 + 1 + 1 + sign->size();
        }

        static FileMeta *
        createNewFileMeta(uint64_t chunksCount, uint32_t chunkSize, uint8_t minSwarmSize, uint8_t ringConnectivity);
    };
}

#endif //RINGSWARM_FILEMETA_H
