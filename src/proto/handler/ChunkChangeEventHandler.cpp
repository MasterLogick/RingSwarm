#include "../ClientHandler.h"
#include "../ServerHandler.h"

#define MAX_RESPONSE_SIZE (0)

namespace RingSwarm::proto {
std::shared_ptr<async::Future<void>> ClientHandler::chunkChangeEvent(
    core::Id *keyId,
    uint64_t chunkIndex,
    uint8_t changeType
) {
    RequestBuffer req(41);
    req.write(keyId);
    req.write<uint64_t>(chunkIndex);
    req.write<uint8_t>(changeType);
    return transport->sendRequest(12, req, MAX_RESPONSE_SIZE)
        ->then([](ResponseHeader) {
        // todo check results
        });
}

void ServerHandler::handleChunkChangeEvent(
    transport::Buffer &request,
    uint8_t tag
) {
    transport->sendEmptyResponse(1, tag);
}
}// namespace RingSwarm::proto
