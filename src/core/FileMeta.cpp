#include "FileMeta.h"
#include <chrono>
#include "Node.h"
#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/HashCrypto.h"

namespace RingSwarm::core {

    FileMeta *
    FileMeta::createNewFileMeta(uint64_t chunksCount, uint32_t chunkSize, uint8_t minSwarmSize,
                                uint8_t ringConnectivity) {
        uint64_t millisecondsTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        struct PackedFileMeta {
            uint8_t author[32];
            uint64_t creationTimestamp;
            uint64_t chunksCount;
            uint32_t chunkSize;
            uint8_t minSwarmSize;
            uint8_t ringConnectivity;
        } __attribute__((packed)) packedFileMeta{{}, millisecondsTimestamp, chunksCount, chunkSize, minSwarmSize,
                                                 ringConnectivity};
        memcpy(&packedFileMeta.author, Node::thisNode->id->hash, 32);
        auto sign = crypto::signData(&packedFileMeta, sizeof(PackedFileMeta));
        uint8_t meta[sizeof(PackedFileMeta) + sign->size()];
        memcpy(meta, &packedFileMeta, sizeof(PackedFileMeta));
        memcpy(meta + sizeof(PackedFileMeta), sign->data(), sign->size());
        auto *hash = crypto::hashData(meta, sizeof(PackedFileMeta) + sign->size());
        return new FileMeta(core::Node::thisNode->id, millisecondsTimestamp, chunksCount, chunkSize, minSwarmSize,
                            ringConnectivity, sign, hash);

    }
}