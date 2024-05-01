#ifndef RINGSWARM_CHUNKRING_H
#define RINGSWARM_CHUNKRING_H

#include "Node.h"

#include <map>

namespace RingSwarm::core {
using ChunkRing = std::map<uint64_t, std::vector<Node *>>;
}

#endif// RINGSWARM_CHUNKRING_H
