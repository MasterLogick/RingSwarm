#ifndef RINGSWARM_FILEMETASWARM_H
#define RINGSWARM_FILEMETASWARM_H

#include <map>
#include <vector>
#include "FileMeta.h"
#include "Node.h"
#include "Swarm.h"

namespace RingSwarm::core {
    class FileMetaSwarm : public Swarm {
        core::FileMeta *meta;
        std::map<uint64_t, std::vector<std::shared_ptr<Node>>> connectedChunkSwarms;
    public:
        FileMetaSwarm(core::FileMeta *meta, std::vector<Node *> swarm) {}

        core::FileMeta *getFileMeta();

        std::map<uint64_t, std::vector<std::shared_ptr<Node>>> &getConnectedChunkSwarms();
    };
}

#endif //RINGSWARM_FILEMETASWARM_H
