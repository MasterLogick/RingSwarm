#ifndef RINGSWARM_CHUNKSWARM_H
#define RINGSWARM_CHUNKSWARM_H

#include "ChunkLink.h"
#include "ChunkRing.h"
#include "Id.h"
#include "Node.h"

#include <map>
#include <string>
#include <vector>

namespace RingSwarm::core {
struct ChunkSwarm {
    ChunkLink *link;
    ChunkRing *ring;
};
}// namespace RingSwarm::core

#endif// RINGSWARM_CHUNKSWARM_H
