#ifndef RINGSWARM_FILEHANDLER_H
#define RINGSWARM_FILEHANDLER_H

#include "../core/FileMeta.h"
#include "../proto/ClientHandler.h"

namespace RingSwarm::client {
    class FileHandler {
        core::FileMeta *meta;
        proto::ClientHandler *possibleFileMetaNode;
    public:
        FileHandler(core::FileMeta *meta, proto::ClientHandler *possibleFileMetaNode);
    };
}

#endif //RINGSWARM_FILEHANDLER_H
