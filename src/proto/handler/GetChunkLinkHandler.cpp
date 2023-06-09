#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<core::ChunkLink *>>
    ClientHandler::getChunkLink(core::Id *keyId, uint64_t chunkIndex) {
        RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        return transport->sendRequest(3, req, MAX_RESPONSE_LENGTH)->then<core::ChunkLink *>([&](ResponseHeader h) {
            return transport->readBuffer(h.responseLen)->then<core::ChunkLink *>([](transport::Buffer resp) {
                return resp.read<core::ChunkLink *>();
            });
        });

    }

    void ServerHandler::handleGetChunkLink(transport::Buffer &request, uint8_t tag) {
        auto *keyId = request.read<core::Id *>();
        uint64_t chunkIndex = request.read<uint64_t>();
        auto chunk = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunk == nullptr) {
            transport->sendError(tag);
            return;
        }
        auto *link = chunk->link;
        ResponseBuffer resp(link->getSerializedSize());
        resp.write(link);
        transport->sendResponse(resp, 1, tag);
    }
}