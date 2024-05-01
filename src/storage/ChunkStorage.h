#ifndef RINGSWARM_CHUNKSTORAGE_H
#define RINGSWARM_CHUNKSTORAGE_H

#include "../core/Id.h"

#include "MappedChunk.h"

#include <memory>

namespace RingSwarm::storage {
std::shared_ptr<MappedChunk>
getMappedChunk(core::Id *keyId, uint64_t chunkIndex);
}

#endif// RINGSWARM_CHUNKSTORAGE_H
