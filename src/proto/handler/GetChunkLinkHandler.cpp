#include "../ServerHandler.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    boost::asio::awaitable<core::ChunkLink *> ClientHandler::getChunkLink(core::Id &id, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(id);
        req.writeUint64(chunkIndex);
        co_await transport->sendRequest(3, req);
        auto resp = co_await transport->readResponse(MAX_RESPONSE_LENGTH);
        co_return resp.readChunkLink();
    }

    boost::asio::awaitable<void> ServerHandler::handleGetChunkLink(transport::Buffer &request) {
        auto *id = request.readId();
        uint64_t chunkIndex = request.readUint64();
        auto chunk = storage::getHostedChunkSwarm(id, chunkIndex);
        if (chunk == nullptr) {
            co_await transport->sendError();
            co_return;
        }
        auto &link = chunk->getChunkLink();
        transport::ResponseBuffer resp(link.getSerializedSize());
        resp.writeChunkLink(link);
        co_await transport->sendResponse(resp);
    }
}