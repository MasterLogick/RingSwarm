#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    core::ChunkLink *ClientHandler::getChunkLink(core::Id *keyId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(3, req);
        auto resp = transport->readResponse(MAX_RESPONSE_LENGTH);
        return resp.read<core::ChunkLink *>();
    }

    void ServerHandler::handleGetChunkLink(transport::Buffer &request) {
        auto *keyId = request.read<core::Id *>();
        uint64_t chunkIndex = request.read<uint64_t>();
        auto chunk = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunk == nullptr) {
            transport->sendError();
            return;
        }
        auto *link = chunk->link;
        transport::ResponseBuffer resp(link->getSerializedSize());
        resp.write(link);
        transport->sendResponse(resp);
    }
}