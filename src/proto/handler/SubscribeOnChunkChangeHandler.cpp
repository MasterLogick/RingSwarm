#include "../ServerHandler.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<void>> ClientHandler::subscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex) {
        RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        return transport->sendRequest(11, req, MAX_RESPONSE_SIZE)->then([](ResponseHeader h) {
            //todo check results
        });
    }

    void ServerHandler::handleSubscribeOnChunkChange(transport::Buffer &request, uint8_t tag) {
        transport->sendEmptyResponse(1, tag);
    }
}