#ifndef RINGSWARM_CHUNKRINGSTORAGE_H
#define RINGSWARM_CHUNKRINGSTORAGE_H

#include "../core/ChunkRing.h"
#include "../core/Node.h"
#include <map>

namespace RingSwarm::storage {
core::ChunkRing *getChunkRing(core::Id *keyId);

void storeChunkRing(core::Id *keyId, core::ChunkRing *ring);
}// namespace RingSwarm::storage

#endif//RINGSWARM_CHUNKRINGSTORAGE_H
