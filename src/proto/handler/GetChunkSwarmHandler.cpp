#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<std::vector<core::Node *>>> ClientHandler::getChunkSwarm(
            core::Id *keyId, uint64_t chunkIndex) {
        RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        return transport->sendRequest(9, req, MAX_RESPONSE_SIZE)->then<std::vector<core::Node *>>([&](
                ResponseHeader h) {
            return transport->readBuffer(h.responseLen)->then<std::vector<core::Node *>>(
                    [](transport::Buffer resp) {
                        return resp.readVec<core::Node *>();
                    });
        });
    }

    void ServerHandler::handleGetChunkSwarm(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError(tag);
        } else {
            sendNodeListResponse((*chunkSwarm->ring)[chunkIndex], 1, tag);
        }
    }
}