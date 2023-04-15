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
    class ChunkSwarm : public Swarm {
        std::string linkSign;
        std::map<uint64_t, std::vector<std::shared_ptr<Node>>> ringConnections;
        ChunkLink link;
    public:
        const Id &getDataHash() {
            return link.dataHash;
        }

        std::string &getLinkSign() {
            return linkSign;
        }

        ChunkLink &getChunkLink();

        std::map<uint64_t, std::vector<std::shared_ptr<Node>>> &getRingConnections();

        const Id &getFileId();
    };
}

#endif //RINGSWARM_CHUNKSWARM_H
