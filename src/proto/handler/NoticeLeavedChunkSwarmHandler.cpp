#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<void>> ClientHandler::noticeLeavedChunkSwarm(core::Id *keyId, uint64_t chunkIndex) {
        RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        return transport->sendRequest(10, req, MAX_RESPONSE_SIZE)->then([](ResponseHeader h) {
            //todo check results
        });
    }

    void ServerHandler::handleNoticeLeavedChunkSwarm(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError(tag);
        } else {
            auto &nodeList = (*chunkSwarm->ring)[chunkIndex];
            if (std::erase_if(nodeList, [this](auto node) -> bool {
                return node == remote;
            }) == 0) {
                transport->sendError(tag);
            } else {
                transport->sendEmptyResponse(1, tag);
            }
        }
    }
}