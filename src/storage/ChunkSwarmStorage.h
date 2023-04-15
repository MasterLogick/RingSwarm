#ifndef RINGSWARM_CHUNKSWARMSTORAGE_H
#define RINGSWARM_CHUNKSWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/ChunkSwarm.h"

namespace RingSwarm::storage {
    core::ChunkSwarm *getHostedChunkSwarm(core::Id *id, uint64_t index);

    core::ChunkSwarm *getNearestHostedChunkSwarm(core::Id *id, uint64_t index);
}
#endif //RINGSWARM_CHUNKSWARMSTORAGE_H
