#ifndef RINGSWARM_FILEMETASWARM_H
#define RINGSWARM_FILEMETASWARM_H

#include <map>
#include <vector>
#include "FileMeta.h"
#include "Node.h"
#include "Swarm.h"

namespace RingSwarm::core {
    struct FileMetaSwarm : public Swarm {
        core::FileMeta *meta;
        uint8_t index;
        std::map<uint64_t, std::vector<Node *>> connectedChunkSwarms;

        FileMetaSwarm(core::FileMeta *meta, uint8_t index) : meta(meta), index(index) {

        }

        FileMetaSwarm(core::FileMeta *meta, uint8_t index, std::vector<Node *> &swarm) :
                meta(meta), index(index), Swarm(swarm) {

        }
    };
}

#endif //RINGSWARM_FILEMETASWARM_H
