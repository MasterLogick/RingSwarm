#include "FileMetaSwarm.h"

namespace RingSwarm::core {
    core::FileMeta *FileMetaSwarm::getFileMeta() {
        return meta;
    }

    std::map<uint64_t, std::vector<std::shared_ptr<Node>>> &FileMetaSwarm::getConnectedChunkSwarms() {
        return connectedChunkSwarms;
    }
}