#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"
#include "../ServerHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
std::shared_ptr<async::Future<core::ChunkLink *>>
ClientHandler::getChunkLink(core::Id *keyId, uint64_t chunkIndex) {
    RequestBuffer req(40);
    req.write(keyId);
    req.write<uint64_t>(chunkIndex);
    return transport->sendShortRequest(3, req, MAX_RESPONSE_LENGTH)->then<core::ChunkLink *>([](auto resp) {
        return resp->template read<core::ChunkLink *>();
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
    auto resp = std::make_shared<ResponseBuffer>(link->getSerializedSize());
    resp->write(link);
    transport->scheduleResponse(std::move(resp), 1, tag);
}
}// namespace RingSwarm::proto