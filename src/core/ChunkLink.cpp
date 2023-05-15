#include "ChunkLink.h"

namespace RingSwarm::core {
    ChunkLink *ChunkLink::createChunkLink(Id *file, uint64_t chunkIndex, Id *dataHash) {
        struct PackedChunkLink {
            uint8_t file[32];
            uint64_t chunkIndex;
            uint8_t dataHash[32];
        } __attribute__((packed)) packedChunkLink{{}, chunkIndex, {}};
        memcpy(&packedChunkLink.file, file->hash, 32);
        memcpy(&packedChunkLink.dataHash, dataHash->hash, 32);
        auto sign = crypto::signData(&packedChunkLink, sizeof(PackedChunkLink));
        return new ChunkLink(file, chunkIndex, dataHash, sign);
    }
}