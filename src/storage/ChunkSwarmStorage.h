#ifndef RINGSWARM_CHUNKSWARMSTORAGE_H
#define RINGSWARM_CHUNKSWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/ChunkSwarm.h"

namespace RingSwarm::storage {
    core::ChunkSwarm *getHostedChunkSwarm(core::Id *keyId, uint64_t index);

    core::Node *getNearestChunkNode(core::Id *keyId, uint64_t chunkIndex, uint8_t *retSwarmIndex);

    void storeChunkSwarm(core::ChunkSwarm *chunkSwarm);
}
#endif //RINGSWARM_CHUNKSWARMSTORAGE_H
