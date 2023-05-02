#include <map>
#include <vector>
#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../../core/Random.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (1024*8)
namespace RingSwarm::proto {

    uint64_t ClientHandler::getNearestChunk(core::Id *fileId, uint64_t chunkIndex, core::Node **node) {
        transport::RequestBuffer req(32 + 8);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        transport->sendRequest(2, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        auto swarmIndex = resp.readUint64();
        *node = resp.readNode();
        return swarmIndex;
    }

    void ServerHandler::handleGetNearestChunk(transport::Buffer &request) {
        auto fileId = request.readId();
        auto chunkIndex = request.readUint64();
        uint8_t chunk;
        auto *node = storage::getNearestChunkNode(fileId, chunkIndex, &chunk);
        if (node != nullptr) {
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.writeUint8(chunk);
            resp.writeNode(node);
            transport->sendResponse(resp);
        } else {
            transport->sendError();
        }
    }
}