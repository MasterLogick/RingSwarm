#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (1024*8)
namespace RingSwarm::proto {

    uint64_t ClientHandler::getNearestChunk(core::Id *keyid, uint64_t chunkIndex, core::Node **node) {
        transport::RequestBuffer req(32 + 8);
        req.write(keyid);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(2, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        auto swarmIndex = resp.read<uint64_t>();
        *node = resp.read<core::Node *>();
        return swarmIndex;
    }

    void ServerHandler::handleGetNearestChunk(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        uint8_t chunk;
        auto *node = storage::getNearestChunkNode(keyId, chunkIndex, &chunk);
        if (node != nullptr) {
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.write<uint8_t>(chunk);
            resp.write(node);
            transport->sendResponse(resp);
        } else {
            transport->sendError();
        }
    }
}