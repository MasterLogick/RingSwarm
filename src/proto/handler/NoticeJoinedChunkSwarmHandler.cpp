#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::noticeJoinedChunkSwarm(core::Id &fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        transport->sendRequest(7, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.readNodeList();
    }

    void ServerHandler::handleNoticeJoinedChunkSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto chunkIndex = request.readUint64();
        auto chunkSwarm = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunkSwarm == nullptr) {
            transport->sendError();
        } else {
            auto nodeList = chunkSwarm->getSwarmNodes();
            if (std::none_of(nodeList.begin(), nodeList.end(),
                             [&](auto node) -> bool { return node == remote; })) {
                nodeList.push_back(remote);
            }
            sendNodeListResponse(nodeList);
        }
    }
}