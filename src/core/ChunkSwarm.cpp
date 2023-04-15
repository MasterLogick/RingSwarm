#include "ChunkSwarm.h"

namespace RingSwarm::core {

    std::map<uint64_t, std::vector<std::shared_ptr<Node>>> &ChunkSwarm::getRingConnections() {
        return ringConnections;
    }

    core::ChunkLink &ChunkSwarm::getChunkLink() {
        return link;
    }

    const Id &ChunkSwarm::getFileId() {
        return link.file;
    }
}