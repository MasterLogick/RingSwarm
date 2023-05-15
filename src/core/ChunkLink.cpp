#include "ChunkLink.h"

namespace RingSwarm::core {
    ChunkLink *ChunkLink::createChunkLink(crypto::KeyPair &keyPair, uint64_t chunkIndex, Id *dataHash) {
        struct PackedChunkLink {
            uint8_t keyHash[32];
            uint64_t chunkIndex;
            uint8_t dataHash[32];
        } __attribute__((packed)) packedChunkLink{{}, chunkIndex, {}};
        auto *keyId = keyPair.publicKey->getId();
        memcpy(&packedChunkLink.keyHash, keyId->hash, 32);
        memcpy(&packedChunkLink.dataHash, dataHash->hash, 32);
        auto sign = keyPair.signData(&packedChunkLink, sizeof(PackedChunkLink));
        return new ChunkLink(keyId, chunkIndex, dataHash, sign);
    }
}