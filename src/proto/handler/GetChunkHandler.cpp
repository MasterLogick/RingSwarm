#include "../ServerHandler.h"
#include "../ClientHandler.h"

namespace RingSwarm::proto {
    boost::asio::awaitable<void> ClientHandler::getChunk(
            core::Id &fileId,
            uint64_t chunkIndex,
            uint32_t offset,
            void *buffer,
            uint32_t length
    ) {
        co_return;
    }

    boost::asio::awaitable<void> ServerHandler::handleGetChunk(transport::Buffer &request) {
        co_await transport->sendEmptyResponse();
    }
}