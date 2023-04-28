#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    core::ChunkLink *ClientHandler::getChunkLink(core::Id &id, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(id);
        req.writeUint64(chunkIndex);
        transport->sendRequest(3, req);
        auto resp = transport->readResponse(MAX_RESPONSE_LENGTH);
        return resp.readChunkLink();
    }

    void ServerHandler::handleGetChunkLink(transport::Buffer &request) {
        auto *id = request.readId();
        uint64_t chunkIndex = request.readUint64();
        auto chunk = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunk == nullptr) {
            transport->sendError();
            return;
        }
        auto &link = chunk->getChunkLink();
        transport::ResponseBuffer resp(link.getSerializedSize());
        resp.writeChunkLink(link);
        transport->sendResponse(resp);
    }
}