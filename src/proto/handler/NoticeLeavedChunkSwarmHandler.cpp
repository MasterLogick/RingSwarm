#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::noticeLeavedChunkSwarm(core::Id *keyId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(10, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    core::Node *ClientHandler::getRemote() {
        return remote;
    }

    void ServerHandler::handleNoticeLeavedChunkSwarm(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError();
        } else {
            auto &nodeList = (*chunkSwarm->ring)[chunkIndex];
            if (std::erase_if(nodeList, [&](auto node) -> bool {
                return node == remote;
            }) == 0) {
                transport->sendError();
            } else {
                transport->sendEmptyResponse();
            }
        }
    }
}