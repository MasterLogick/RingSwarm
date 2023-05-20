#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::getChunkSwarm(core::Id *keyId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(9, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.readVec<core::Node *>();
    }

    void ServerHandler::handleGetChunkSwarm(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError();
        } else {
            sendNodeListResponse((*chunkSwarm->ring)[chunkIndex]);
        }
    }
}