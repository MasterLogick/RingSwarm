#ifndef RINGSWARM_FILEMETASWARM_H
#define RINGSWARM_FILEMETASWARM_H

#include <map>
#include <vector>
#include "FileMeta.h"
#include "Node.h"
#include "ChunkRing.h"

namespace RingSwarm::core {
    struct FileMetaSwarm {
        core::FileMeta *meta;
        std::map<int, Node *> swarm;
        ChunkRing *ring;
    };
}

#endif //RINGSWARM_FILEMETASWARM_H
