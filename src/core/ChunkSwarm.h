#ifndef RINGSWARM_CHUNKSWARM_H
#define RINGSWARM_CHUNKSWARM_H

#include <string>
#include <map>
#include <vector>
#include "Id.h"
#include "ChunkLink.h"
#include "Node.h"
#include "Swarm.h"

namespace RingSwarm::core {
    struct ChunkSwarm : public Swarm {
        std::map<uint64_t, std::vector<Node *>> ringConnections;
        ChunkLink link;

        const Id &getDataHash() {
            return link.dataHash;
        }
    };
}

#endif //RINGSWARM_CHUNKSWARM_H
