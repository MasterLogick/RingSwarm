#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    boost::asio::awaitable<std::vector<core::Node *>>
    ClientHandler::getChunkSwarm(core::Id &fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        transport->sendRequest(9, req);
        auto resp = co_await transport->readResponse(MAX_RESPONSE_SIZE);
        co_return resp.readNodeList();
    }

    boost::asio::awaitable<void> ServerHandler::handleGetChunkSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto chunkIndex = request.readUint64();
        auto chunkSwarm = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunkSwarm == nullptr) {
            co_await transport->sendError();
        } else {
            co_await sendNodeListResponse(chunkSwarm->getSwarmNodes());
        }
    }
}