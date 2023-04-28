#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    boost::asio::awaitable<std::vector<core::Node *>>
    ClientHandler::noticeJoinedChunkSwarm(core::Id &fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        co_await transport->sendRequest(7, req);
        auto resp = co_await transport->readResponse(MAX_RESPONSE_SIZE);
        co_return resp.readNodeList();
    }

    boost::asio::awaitable<void> ServerHandler::handleNoticeJoinedChunkSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto chunkIndex = request.readUint64();
        auto chunkSwarm = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunkSwarm == nullptr) {
            co_await transport->sendError();
        } else {
            auto nodeList = chunkSwarm->getSwarmNodes();
            if (std::none_of(nodeList.begin(), nodeList.end(),
                             [&](auto node) -> bool { return node == remote; })) {
                nodeList.push_back(remote);
            }
            co_await sendNodeListResponse(nodeList);
        }
    }
}