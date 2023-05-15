#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::getChunkSwarm(core::Id *fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(fileId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(9, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.read<std::vector<core::Node *>>();
    }

    void ServerHandler::handleGetChunkSwarm(transport::Buffer &request) {
        auto id = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError();
            sendNodeListResponse((*chunkSwarm->ring)[chunkIndex]);
        }
    }
}