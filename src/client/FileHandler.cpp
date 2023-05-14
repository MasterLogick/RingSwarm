#include "FileHandler.h"

namespace RingSwarm::client {

    FileHandler::FileHandler(core::FileMeta *meta, proto::ClientHandler *possibleFileMetaNode) :
            meta(meta), possibleFileMetaNode(possibleFileMetaNode) {

    }
}