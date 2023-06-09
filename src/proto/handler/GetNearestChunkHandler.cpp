#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (1024 * 8)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<core::Node *, uint64_t>>
    ClientHandler::getNearestChunk(core::Id *keyid, uint64_t chunkIndex) {
        RequestBuffer req(32 + 8);
        req.write(keyid);
        req.write<uint64_t>(chunkIndex);
        auto f = async::Future<core::Node *, uint64_t>::create();
        transport->sendRequest(2, req, MAX_RESPONSE_SIZE)->then([&](ResponseHeader h) {
            transport->readBuffer(h.responseLen)->then([f](transport::Buffer resp) {
                auto swarmIndex = resp.read<uint64_t>();
                auto *node = resp.read<core::Node *>();
                f->resolve(node, swarmIndex);
            });
        });
        return f;
    }

    void ServerHandler::handleGetNearestChunk(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        uint8_t chunk;
        auto *node = storage::getNearestChunkNode(keyId, chunkIndex, &chunk);
        if (node != nullptr) {
            ResponseBuffer resp(1 + node->getSerializedSize());
            resp.write<uint8_t>(chunk);
            resp.write(node);
            transport->sendResponse(resp, 1, tag);
        } else {
            transport->sendError(tag);
        }
    }
}