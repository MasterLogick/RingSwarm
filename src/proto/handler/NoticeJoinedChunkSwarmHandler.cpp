#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<std::vector<core::Node *>>> ClientHandler::noticeJoinedChunkSwarm(core::Id *keyId,
                                                                                                    uint64_t chunkIndex) {
        RequestBuffer req(40);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        return transport->sendShortRequest(7, req, MAX_RESPONSE_SIZE)->
                then<std::vector<core::Node *>>([](auto resp) {
            return resp->template readVec<core::Node *>();

        });
    }

    void ServerHandler::handleNoticeJoinedChunkSwarm(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto chunkSwarm = storage::getHostedChunkSwarm(keyId, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError(tag);
        } else {
            auto &nodeList = (*chunkSwarm->ring)[chunkIndex];
            if (std::none_of(nodeList.begin(), nodeList.end(),
                             [this](auto node) -> bool { return node == remote; })) {
                nodeList.push_back(remote);
            }
            sendNodeListResponse(nodeList, 1, tag);
        }
    }
}