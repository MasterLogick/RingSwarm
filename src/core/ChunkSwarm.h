#ifndef RINGSWARM_CHUNKSWARM_H
#define RINGSWARM_CHUNKSWARM_H

#include <string>
#include <map>
#include <vector>
#include "Id.h"
#include "ChunkLink.h"
#include "Node.h"
#include "ChunkRing.h"

namespace RingSwarm::core {
    struct ChunkSwarm {
        ChunkLink *link;
        ChunkRing *ring;
    };
}

#endif //RINGSWARM_CHUNKSWARM_H
