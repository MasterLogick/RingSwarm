#include "../ServerHandler.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    boost::asio::awaitable<void> ClientHandler::chunkChangeEvent(core::Id &fileId, uint64_t chunkIndex, uint8_t changeType) {
        transport::RequestBuffer req(41);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        req.writeUint8(changeType);
        transport->sendRequest(12, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    boost::asio::awaitable<void> ServerHandler::handleChunkChangeEvent(transport::Buffer &request) {
        co_await transport->sendEmptyResponse();
    }
}