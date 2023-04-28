#include "../ServerHandler.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    boost::asio::awaitable<void> ClientHandler::unsubscribeOnChunkChange(core::Id &fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        co_await transport->sendRequest(12, req);
        co_await transport->readResponse(MAX_RESPONSE_SIZE);
    }

    boost::asio::awaitable<void> ServerHandler::handleUnsubscribeOnChunkChange(transport::Buffer &request) {
        co_await transport->sendEmptyResponse();
    }
}