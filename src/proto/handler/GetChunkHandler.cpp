#include "../ServerHandler.h"
#include "../ClientHandler.h"

namespace RingSwarm::proto {
    void ClientHandler::getChunk(
            core::Id *fileId,
            uint64_t chunkIndex,
            uint32_t offset,
            void *buffer,
            uint32_t length
    ) {

    }

    void ServerHandler::handleGetChunk(transport::Buffer &request) {
        transport->sendEmptyResponse();
    }
}