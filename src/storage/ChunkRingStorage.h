#ifndef RINGSWARM_CHUNKRINGSTORAGE_H
#define RINGSWARM_CHUNKRINGSTORAGE_H

#include <map>
#include "../core/Node.h"
#include "../core/ChunkRing.h"

namespace RingSwarm::storage {
    core::ChunkRing *getChunkRing(core::Id *keyId);

    void storeChunkRing(core::Id *keyId, core::ChunkRing *ring);
}

#endif //RINGSWARM_CHUNKRINGSTORAGE_H
