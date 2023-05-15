#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::noticeJoinedChunkSwarm(core::Id *keyId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(7, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.read<std::vector<core::Node *>>();
    }

    void ServerHandler::handleNoticeJoinedChunkSwarm(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError();
        } else {
            auto &nodeList = (*chunkSwarm->ring)[chunkIndex];
            if (std::none_of(nodeList.begin(), nodeList.end(),
                             [&](auto node) -> bool { return node == remote; })) {
                nodeList.push_back(remote);
            }
            sendNodeListResponse(nodeList);
        }
    }
}