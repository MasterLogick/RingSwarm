#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    boost::asio::awaitable<void> ClientHandler::noticeLeavedChunkSwarm(core::Id &fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        co_await transport->sendRequest(10, req);
        co_await transport->readResponse(MAX_RESPONSE_SIZE);
    }

    boost::asio::awaitable<void> ServerHandler::handleNoticeLeavedChunkSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto chunkIndex = request.readUint64();
        auto chunkSwarm = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunkSwarm == nullptr) {
            co_await transport->sendError();
        } else {
            auto nodeList = chunkSwarm->getSwarmNodes();
            if (std::erase_if(nodeList, [&](auto node) -> bool {
                return node == remote;
            }) == 0) {
                co_await transport->sendError();
            } else {
                co_await transport->sendEmptyResponse();
            }
        }
    }
}